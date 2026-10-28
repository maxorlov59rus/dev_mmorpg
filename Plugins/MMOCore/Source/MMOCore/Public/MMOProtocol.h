// Copyright BlackMatter Team. All Rights Reserved.
//
// Бинарный протокол MMOCore — формат пакета и таблица opcode'ов.
// Единый источник истины: /proto/opcodes.defs (репозиторий). Здесь — зеркало.
//
// Фрейм на проводе (little-endian, как в TrinityCore):
// +----------------+----------------+-------------------+--------------+
// | uint16 Size    | uint16 Opcode  | payload[Size-4]   | (padding)    |
// +----------------+----------------+-------------------+--------------+
// Size = заголовок(4) + payload. Максимальный пакет: MMOCORE_MAX_PACKET.

#pragma once

#include "CoreMinimal.h"
#include "MMOProtocol.generated.h"

/** Hard limit размера одного фрейма (защита от битого потока). TRC-style: 512 KiB. */
static constexpr uint32 MMOCORE_MAX_PACKET_SIZE = 512 * 1024;
static constexpr uint32 MMOCORE_HEADER_SIZE     = sizeof(uint16) + sizeof(uint16); // 4

/**
 * Таблица сообщений клиента/сервера. Нумерация — блок-ориентированная,
 * как в TrinityCore (MessageHash->Opcode): нижние байты = индекс внутри блока.
 */
UENUM(BlueprintType)
enum class EMMOOpcode : uint16
{
	// ---- Connection / keepalive (0x000x) ----
	None                = 0x0000 UMETA(DisplayName = "None"),
	CmdPing             = 0x0001 UMETA(DisplayName = "Ping"),
	SrvPong             = 0x0002 UMETA(DisplayName = "Pong"),
	CmdDisconnect       = 0x0003 UMETA(DisplayName = "Disconnect"),
	SrvServerStatus     = 0x0004 UMETA(DisplayName = "ServerStatus"),

	// ---- Auth block (0x01xx) — authserver (SRP-style handshake) ----
	CmdAuthStart        = 0x0101 UMETA(DisplayName = "AuthStart"),        // login hello
	SrvAuthChallenge    = 0x0102 UMETA(DisplayName = "AuthChallenge"),    // salt+B
	CmdAuthConfirm      = 0x0103 UMETA(DisplayName = "AuthConfirm"),      // M1 proof
	SrvAuthResponse     = 0x0104 UMETA(DisplayName = "AuthResponse"),     // M2 + sessionKey flag
	CmdAuthSessionTicket= 0x0105 UMETA(DisplayName = "AuthSessionTicket"),// -> loginserver
	SrvAuthSessionResult= 0x0106 UMETA(DisplayName = "AuthSessionResult"),

	// ---- Login / realm block (0x02xx) — loginserver: список шардов ----
	CmdCharListRequest  = 0x0201 UMETA(DisplayName = "CharListRequest"),
	SrvCharList         = 0x0202 UMETA(DisplayName = "CharList"),
	CmdCharCreate       = 0x0203 UMETA(DisplayName = "CharCreate"),
	CmdCharDelete       = 0x0204 UMETA(DisplayName = "CharDelete"),
	CmdEnterWorld       = 0x0205 UMETA(DisplayName = "EnterWorld"),
	SrvWorldServerInfo  = 0x0206 UMETA(DisplayName = "WorldServerInfo"), // IP/port mapserver ticket

	// ---- World block (0x03xx) — worldserver ----
	CmdPlayerLogin      = 0x0301 UMETA(DisplayName = "PlayerLogin"),
	SrvLoginComplete    = 0x0302 UMETA(DisplayName = "LoginComplete"),
	SrvLogoutComplete   = 0x0303 UMETA(DisplayName = "LogoutComplete"),
	CmdSetActiveObject  = 0x0304 UMETA(DisplayName = "SetActiveObject"),
	SrvTutorialFlags    = 0x0305 UMETA(DisplayName = "TutorialFlags"),

	// ---- Movement / sync (0x04xx) ----
	CmdPlayerMove       = 0x0401 UMETA(DisplayName = "PlayerMove"),
	SrvObjectsUpdate    = 0x0402 UMETA(DisplayName = "ObjectsUpdate"),
	SrvValuesUpdate     = 0x0403 UMETA(DisplayName = "ValuesUpdate"),

	// ---- Combat / factions (0x05xx) ----
	CmdAttack           = 0x0501 UMETA(DisplayName = "Attack"),
	SrvAttackResult     = 0x0502 UMETA(DisplayName = "AttackResult"),
	SrvReputationUpdate = 0x0503 UMETA(DisplayName = "ReputationUpdate"),

	// ---- Chat (0x06xx) — отдельный chat-сервер принимает тот же фрейминг ----
	CmdChatMessage      = 0x0601 UMETA(DisplayName = "ChatMessage"),
	SrvChatMessage      = 0x0602 UMETA(DisplayName = "ChatMessageSrv"),

	Max                 = 0xFFFF UMETA(Hidden),
};

/** Статусы авторизации (зеркало AuthResults из TRC). */
UENUM(BlueprintType)
enum class EMMOAuthStatus : uint8
{
	Success            = 0  UMETA(DisplayName = "Success"),
	UnknownAccount     = 1  UMETA(DisplayName = "UnknownAccount"),
	BadPassword        = 2  UMETA(DisplayName = "BadPassword"),
	Failed             = 3  UMETA(DisplayName = "ConnectionFailed"),
	AlreadyOnline      = 4  UMETA(DisplayName = "AlreadyOnline"),
	Banned             = 7  UMETA(DisplayName = "Banned"),
	WrongPass          = 14 UMETA(DisplayName = "WrongPass"),
	SessionExpired     = 15 UMETA(DisplayName = "SessionExpired"),
};

/** Состояние TCP-соединения клиента. */
UENUM(BlueprintType)
enum class EMMONetConnectionState : uint8
{
	Disconnected,
	Connecting,
	Connected,        // транспорт установлен, но ещё не аутентифицирован
	Authenticating,
	Authenticated,
	Failing,          // graceful shutdown initiated
};

/**
 * Лёгкий биначный пакет: заголовок + payload. Не UObject для скорости;
 * для Blueprint-экспозиции используется обёртка FMMOPacketBP (см. MMOGameInstance).
 */
struct FMMOPacket
{
	uint16 Opcode = 0;
	TArray<uint8> Payload;

	FMMOPacket() = default;
	FMMOPacket(EMMOOpcode InOpcode, TArray<uint8>&& InPayload)
		: Opcode(static_cast<uint16>(InOpcode)), Payload(MoveTemp(InPayload)) {}

	// ---------- Serialize ----------
	/** Пишит фрейм [Size|Opcode|Payload] в OutBytes (LE). Возвращает false при переполнении лимита. */
	bool Serialize(TArray<uint8>& OutBytes) const
	{
		const uint32 TotalSize = MMOCORE_HEADER_SIZE + Payload.Num();
		if (TotalSize > MMOCORE_MAX_PACKET_SIZE)
		{
			return false;
		}
		OutBytes.Reset(TotalSize);
		OutBytes.SetNumUninitialized(TotalSize);

		uint8* D = OutBytes.GetData();
		// Size (LE)
		D[0] = static_cast<uint8>(TotalSize & 0xFF);
		D[1] = static_cast<uint8>((TotalSize >> 8) & 0xFF);
		// Opcode (LE)
		D[2] = static_cast<uint8>(Opcode & 0xFF);
		D[3] = static_cast<uint8>((Opcode >> 8) & 0xFF);
		if (Payload.Num() > 0)
		{
			FMemory::Memcpy(D + MMOCORE_HEADER_SIZE, Payload.GetData(), Payload.Num());
		}
		return true;
	}

	/**
	 * Deserialize: читает ОДИН фрейм из начала Buffer.
	 * При успехе сдвигает Buffer (erase 0..FrameSize) и заполняет OutPacket.
	 * Возвращает false если данных пока недостаточно (buffer underrun) —
	 * это НЕ ошибка, caller должен дочитать из сокета и повторить попытку.
	 */
	static bool TryDeserialize(const uint8* Data, int32 Num, FMMOPacket& OutPacket, int32& OutConsumed)
	{
		if (Num < MMOCORE_HEADER_SIZE)
		{
			return false;
		}
		const uint8* S = Data;
		const uint32 FrameSize = static_cast<uint32>(S[0]) | (static_cast<uint32>(S[1]) << 8);
		if (FrameSize < MMOCORE_HEADER_SIZE || FrameSize > MMOCORE_MAX_PACKET_SIZE)
		{
			// поток повреждён — соединение подлежит разрыву
			return false;
		}
		if (static_cast<uint32>(Num) < FrameSize)
		{
			return false; // ждём остаток
		}
		OutPacket.Opcode = static_cast<uint16>(S[2]) | (static_cast<uint16>(S[3]) << 8);
		const int32 PayloadLen = static_cast<int32>(FrameSize) - MMOCORE_HEADER_SIZE;
		OutPacket.Payload.SetNumUninitialized(PayloadLen);
		if (PayloadLen > 0)
		{
			FMemory::Memcpy(OutPacket.Payload.GetData(), S + MMOCORE_HEADER_SIZE, PayloadLen);
		}
		OutConsumed = static_cast<int32>(FrameSize);
		return true;
	}

	// ---------- Payload writer/reader helpers (TRC BigNumber/LittleEndian style) ----------
	void WriteU8(uint8 V)  { Payload.Add(V); }
	void WriteU16(uint16 V){ AppendLE(V); }
	void WriteU32(uint32 V){ AppendLE(V); }
	void WriteU64(uint64 V){ AppendLE(V); }

	void WriteString(const FString& S)
	{
		FTCHARToUTF8 Utf8(*S);
		WriteU32(Utf8.Length());
		Payload.Append(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
	}

	/** Чтектор поверх payload; инкрементальные read'ы. */
	struct FReader
	{
		const uint8* P; int32 N; int32 Pos = 0;
		FReader(const TArray<uint8>& InPayload) : P(InPayload.GetData()), N(InPayload.Num()) {}

		bool ReadU8(uint8& Out)  { if (N - Pos < 1) return false; Out = P[Pos++]; return true; }
		bool ReadU16(uint16& Out){ if (N - Pos < 2) return false; Out = ReadLE<uint16>(); return true; }
		bool ReadU32(uint32& Out){ if (N - Pos < 4) return false; Out = ReadLE<uint32>(); return true; }
		bool ReadU64(uint64& Out){ if (N - Pos < 8) return false; Out = ReadLE<uint64>(); return true; }
		bool ReadString(FString& Out)
		{
			uint32 Len; if (!ReadU32(Len) || Len > static_cast<uint32>(N - Pos)) return false;
			Out = FString(UTF8_TO_TCHAR(reinterpret_cast<const ANSICHAR*>(P + Pos)));
			Pos += Len; return true;
		}
		bool ReadBytes(TArray<uint8>& Out, int32 Count)
		{
			if (Count < 0 || N - Pos < Count) return false;
			Out.Append(P + Pos, Count); Pos += Count; return true;
		}
	private:
		template<typename T> T ReadLE()
		{
			T V = 0;
			for (uint32 i = 0; i < sizeof(T); ++i) V |= static_cast<T>(static_cast<T>(P[Pos + i]) << (8 * i));
			Pos += sizeof(T);
			return V;
		}
	};

private:
	template<typename T> void AppendLE(T V)
	{
		for (uint32 i = 0; i < sizeof(T); ++i)
		{
			Payload.Add(static_cast<uint8>((V >> (8 * i)) & 0xFF));
		}
	}
};
