// Copyright BlackMatter Team. All Rights Reserved.
//
// Пакеты авторизации: CmdAuthStart / SrvAuthChallenge / CmdAuthConfirm / SrvAuthResponse.
// Схема — упрощённый SRP-like handshake (как AuthStart/AuthProof in TrinityCore):
//
//   Client -> Server : AUTH_START    { login }
//   Server -> Client : AUTH_CHALLENGE{ salt[32], serverB[32] }
//   Client -> Server : AUTH_CONFIRM  { clientA[32], clientM1[20] }   // M1 = HMAC(session, ...)
//   Server -> Client : AUTH_RESPONSE { status, sessionKeyFlags, m2[20]?, ticket? }
//
// NOTE(M0): здесь client-side заглушка хеша на FSHA1/FMD5 из UE Core; боевой SRP
// (big-number) приедет вместе с dm-authserver (см. docs/02). Провод форматы совпадают.

#pragma once

#include "CoreMinimal.h"
#include "MMOProtocol.h"

/** Параметры TCP endpoints — читаются из DefaultEngine.ini [/MMOCore.Network]. */
USTRUCT(BlueprintType)
struct FMMOServerEndpoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMO|Net")
	FString Host = TEXT("127.0.0.1");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMO|Net", meta = (ClampMin = "1", ClampMax = "65535"))
	int32 Port = 8085;

	bool IsValid() const { return !Host.IsEmpty() && Port > 0 && Port <= 65535; }
};

/** Запрос начала аутентификации. */
USTRUCT(BlueprintType)
struct FMMOAuthRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) FString Username;
	// Пароль НЕ отправляется по проводу — только производные (SRP). Храним локально для вычислений.
	UPROPERTY(Transient) FString PasswordLocal;

	static constexpr int32 SALT_SIZE = 32;
	static constexpr int32 BIGNUM_SIZE = 32; // A/B
	static constexpr int32 PROOF_SIZE = 20;  // SHA1-based proof, TRC-style

	TArray<uint8> Salt;       // получено от сервера
	TArray<uint8> ServerB;    // публичный ключ сервера
	TArray<uint8> ClientA;    // публичный ключ клиента
	TArray<uint8> ClientM1;   // доказательство знания пароля

	/** Собирает CmdAuthStart. */
	void BuildAuthStart(FMMOPacket& Out) const
	{
		Out = FMMOPacket();
		Out.Opcode = static_cast<uint16>(EMMOOpcode::CmdAuthStart);
		Out.WriteString(Username);
	}

	/** Разбирает SrvAuthChallenge (salt + B). */
	bool ParseAuthChallenge(const FMMOPacket& In)
	{
		FMMOPacket::FReader R(In.Payload);
		Salt.Reset(); ServerB.Reset();
		return R.ReadBytes(Salt, SALT_SIZE) && R.ReadBytes(ServerB, BIGNUM_SIZE);
	}

	/**
	 * Локальная генерация A и M1 (заглушка до боевого SRP big-number):
	 *   A  = repeat(SHA1(username || timestamp)) до 32 байт
	 *   K  = SHA1(password)                       // session key stub
	 *   M1 = SHA1(K || username || salt || A || B)
	 * Боевой вариант заменит это на SRP-6a (dm-authserver, docs/02).
	 */
	void GenerateProofs()
	{
		// --- A ---
		const double Now = FPlatformTime::Seconds();
		FString Seed = FString::Printf(TEXT("%s@%.0f"), *Username, Now);
		FTCHARToUTF8 SeedUtf8(*Seed);
		ClientA.SetNumUninitialized(BIGNUM_SIZE);
		{
			FSHA1 Hash;
			Hash.Update(reinterpret_cast<const uint8*>(SeedUtf8.Get()), SeedUtf8.Length());
			Hash.Final();
			const FSHAHash& H = Hash.GetHash();
			const uint8* Digest = H.Hash; // raw 20 bytes
			for (int32 i = 0; i < BIGNUM_SIZE; ++i)
			{
				ClientA[i] = Digest[i % FSHA1::HashSize];
			}
		}

		// --- K = SHA1(password) ---
		uint8 KeyBuf[FSHA1::HashSize];
		{
			FTCHARToUTF8 PwUtf8(*PasswordLocal);
			FSHA1 PwHash;
			PwHash.Update(reinterpret_cast<const uint8*>(PwUtf8.Get()), PwUtf8.Length());
			PwHash.Final();
			FMemory::Memcpy(KeyBuf, PwHash.GetHash().Hash, FSHA1::HashSize);
		}

		// --- M1 = SHA1(K || username || salt || A || B) ---
		ClientM1.SetNumUninitialized(PROOF_SIZE);
		{
			FTCHARToUTF8 UserUtf8(*Username);
			FSHA1 M1;
			M1.Update(KeyBuf, sizeof(KeyBuf));
			M1.Update(reinterpret_cast<const uint8*>(UserUtf8.Get()), UserUtf8.Length());
			M1.Update(Salt.GetData(), Salt.Num());
			M1.Update(ClientA.GetData(), ClientA.Num());
			M1.Update(ServerB.GetData(), ServerB.Num());
			M1.Final();
			FMemory::Memcpy(ClientM1.GetData(), M1.GetHash().Hash, PROOF_SIZE);
		}
	}

	/** Собирает CmdAuthConfirm. */
	void BuildAuthConfirm(FMMOPacket& Out) const
	{
		Out = FMMOPacket();
		Out.Opcode = static_cast<uint16>(EMMOOpcode::CmdAuthConfirm);
		Out.Payload.Append(ClientA);
		Out.Payload.Append(ClientM1);
	}
};

/** Ответ авторизации. */
USTRUCT(BlueprintType)
struct FMMOAuthResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) EMMOAuthStatus Status = EMMOAuthStatus::Failed;
	UPROPERTY(BlueprintReadOnly) uint32 SessionKeyFlags = 0;
	/** Билет для loginserver'а (аналог AuthSessionTicket в TRC). */
	UPROPERTY(BlueprintReadOnly) TArray<uint8> Ticket;

	bool IsSuccess() const { return Status == EMMOAuthStatus::Success; }

	static bool Parse(const FMMOPacket& In, FMMOAuthResponse& Out)
	{
		FMMOPacket::FReader R(In.Payload);
		uint8 St; uint32 Flags; uint32 TicketLen;
		if (!R.ReadU8(St) || !R.ReadU32(Flags)) return false;
		Out.Status = static_cast<EMMOAuthStatus>(St);
		Out.SessionKeyFlags = Flags;
		if (R.ReadU32(TicketLen) && TicketLen > 0 && TicketLen < MMOCORE_MAX_PACKET_SIZE)
		{
			Out.Ticket.Reset();
			if (!R.ReadBytes(Out.Ticket, static_cast<int32>(TicketLen))) return false;
		}
		return true;
	}

	/** Серверная сторона (для unit-тестов и mock-сервера): собрать SrvAuthResponse. */
	void BuildPacket(FMMOPacket& Out) const
	{
		Out = FMMOPacket();
		Out.Opcode = static_cast<uint16>(EMMOOpcode::SrvAuthResponse);
		Out.WriteU8(static_cast<uint8>(Status));
		Out.WriteU32(SessionKeyFlags);
		Out.WriteU32(Ticket.Num());
		Out.Payload.Append(Ticket);
	}
};
