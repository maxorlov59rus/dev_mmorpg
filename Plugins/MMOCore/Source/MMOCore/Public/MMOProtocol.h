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
static constexpr uint32 MMOCORE_HEADER_SIZE = sizeof(uint16) + sizeof(uint16); // 4

/**
 * Таблица сообщений клиента/сервера. Нумерация — блок-ориентированная,
 * как в TrinityCore (MessageHash->Opcode): нижние байты = индекс внутри блока.
 */
UENUM(BlueprintType)
enum class EMMOOpcode : uint16 {
    // ---- Connection / keepalive (0x000x) ----
    None = 0x0000 UMETA(DisplayName = "None"),
    CmdPing = 0x0001 UMETA(DisplayName = "Ping"),
    SrvPong = 0x0002 UMETA(DisplayName = "Pong"),
    CmdDisconnect = 0x0003 UMETA(DisplayName = "Disconnect"),
    SrvServerStatus = 0x0004 UMETA(DisplayName = "ServerStatus"),

    // ---- Auth block (0x01xx) — authserver (SRP-style handshake) ----
    CmdAuthStart = 0x0101 UMETA(DisplayName = "AuthStart"),                 // login hello
    SrvAuthChallenge = 0x0102 UMETA(DisplayName = "AuthChallenge"),         // salt+B
    CmdAuthConfirm = 0x0103 UMETA(DisplayName = "AuthConfirm"),             // M1 proof
    SrvAuthResponse = 0x0104 UMETA(DisplayName = "AuthResponse"),           // M2 + sessionKey flag
    CmdAuthSessionTicket = 0x0105 UMETA(DisplayName = "AuthSessionTicket"), // -> loginserver
    SrvAuthSessionResult = 0x0106 UMETA(DisplayName = "AuthSessionResult"),

    // ---- Login / realm block (0x02xx) — loginserver: список шардов ----
    CmdCharListRequest = 0x0201 UMETA(DisplayName = "CharListRequest"),
    SrvCharList = 0x0202 UMETA(DisplayName = "CharList"),
    CmdCharCreate =
        0x0203 UMETA(DisplayName = "CharCreate"), // wire: CMSG_CHAR_CREATE (defs 0x0012)
    CmdCharDelete = 0x0204 UMETA(DisplayName = "CharDelete"),
    CmdEnterWorld = 0x0205 UMETA(DisplayName = "EnterWorld"),
    SrvWorldServerInfo = 0x0206 UMETA(DisplayName = "WorldServerInfo"), // IP/port mapserver ticket
    SrvCharCreateSuccess = 0x0207 UMETA(
        DisplayName = "CharCreateSuccess"), // wire: SMSG_CHAR_CREATE_SUCCESS (defs 0x0013)
    SrvCharCreateError = 0x0208 UMETA(
        DisplayName = "CharCreateError"), // wire: SMSG_CHAR_CREATE_ERROR   (defs 0x0017)

    // ---- World block (0x03xx) — worldserver ----
    CmdPlayerLogin = 0x0301 UMETA(DisplayName = "PlayerLogin"),
    SrvLoginComplete = 0x0302 UMETA(DisplayName = "LoginComplete"),
    SrvLogoutComplete = 0x0303 UMETA(DisplayName = "LogoutComplete"),
    CmdSetActiveObject = 0x0304 UMETA(DisplayName = "SetActiveObject"),
    SrvTutorialFlags = 0x0305 UMETA(DisplayName = "TutorialFlags"),

    // ---- Movement / sync (0x04xx) ----
    CmdPlayerMove = 0x0401 UMETA(DisplayName = "PlayerMove"),
    SrvObjectsUpdate = 0x0402 UMETA(DisplayName = "ObjectsUpdate"),
    SrvValuesUpdate = 0x0403 UMETA(DisplayName = "ValuesUpdate"),

    // ---- Combat / factions (0x05xx) ----
    CmdAttack = 0x0501 UMETA(DisplayName = "Attack"),
    SrvAttackResult = 0x0502 UMETA(DisplayName = "AttackResult"),
    SrvReputationUpdate =
        0x0503 UMETA(DisplayName = "ReputationUpdate"), // wire: SMSG_REP_UPDATE (defs 0x0075): u8
                                                        // faction, i32 standing(Q16), u8 rank

    // ---- Faction / Renegade system v1.2 (ReputationMgr.h, sql/upgrades/*) ----
    SrvRenegadeState = 0x0504 UMETA(
        DisplayName = "RenegadeState"), // wire: SMSG_RENEGADE_STATE (defs 0x0076): u8 is_renegade,
                                        // u8 at_war_with_all, u16 black_market_unlocked
    SrvFactionVisual =
        0x0505 UMETA(DisplayName = "FactionVisual"), // wire: SMSG_FACTION_VISUAL (defs 0x0077, map
                                                     // chan) -> FactionFXManager::ApplyToUnit
    SrvCreatureAggro =
        0x0506 UMETA(DisplayName = "CreatureAggro"), // wire: SMSG_CREATURE_AGGRO (defs 0x0078, map
                                                     // chan): guid creature, guid target

    // ---- Chat (0x06xx) — отдельный chat-сервер принимает тот же фрейминг ----
    CmdChatMessage = 0x0601 UMETA(DisplayName = "ChatMessage"),
    SrvChatMessage = 0x0602 UMETA(DisplayName = "ChatMessageSrv"),

    // ---- Quests / Gossip v1.3 (proto/opcodes.defs 0x2000..0x2014) ----
    CmdQuestQuery = 0x0701 UMETA(DisplayName = "QuestQuery"),                   // wire 0x2000
    SrvQuestList = 0x0702 UMETA(DisplayName = "QuestList"),                     // wire 0x2001
    CmdQuestAccept = 0x0703 UMETA(DisplayName = "QuestAccept"),                 // wire 0x2002
    CmdQuestComplete = 0x0704 UMETA(DisplayName = "QuestComplete"),             // wire 0x2003
    SrvQuestAccepted = 0x0705 UMETA(DisplayName = "QuestAccepted"),             // wire 0x2004
    SrvQuestProgress = 0x0706 UMETA(DisplayName = "QuestProgress"),             // wire 0x2005
    SrvQuestCompleteResult = 0x0707 UMETA(DisplayName = "QuestCompleteResult"), // wire 0x2006
    SrvQuestTurnInHint = 0x0708 UMETA(DisplayName = "QuestTurnInHint"),         // wire 0x2007
    CmdGossipHello = 0x0709 UMETA(DisplayName = "GossipHello"),                 // wire 0x2010
    SrvGossipShow = 0x070A UMETA(DisplayName = "GossipShow"),                   // wire 0x2011
    CmdGossipSelect = 0x070B UMETA(DisplayName = "GossipSelect"),               // wire 0x2012
    SrvGossipReply = 0x070C UMETA(DisplayName = "GossipReply"),                 // wire 0x2013
    SrvCreatureText = 0x070D UMETA(DisplayName = "CreatureText"),               // wire 0x2014

    // Classes / Specs / Stats (v1.4, defs 0x0090..0x0093)
    CmdClassInfoQuery = 0x0801 UMETA(DisplayName = "ClassInfoQuery"),   // wire 0x0090
    SrvClassInfo = 0x0802 UMETA(DisplayName = "ClassInfo"),             // wire 0x0091
    SrvInitializeStats = 0x0803 UMETA(DisplayName = "InitializeStats"), // wire 0x0092
    SrvLearnSpell = 0x0804 UMETA(DisplayName = "LearnSpell"),           // wire 0x0093

    // Quests 6-10: Spec selection + LFG (v1.5, defs 0x2020..0x2029)
    CmdJoinLfg = 0x0901 UMETA(DisplayName = "JoinLFG"),                       // wire 0x2020
    SrvLfgQueueState = 0x0902 UMETA(DisplayName = "LfgQueueState"),           // wire 0x2021
    SrvLfgRoleCheck = 0x0903 UMETA(DisplayName = "LfgRoleCheck"),             // wire 0x2022
    SrvSpecSelectionOpen = 0x0904 UMETA(DisplayName = "SpecSelectionOpen"),   // wire 0x2023
    CmdChooseSpec = 0x0905 UMETA(DisplayName = "ChooseSpec"),                 // wire 0x2024
    SrvSpecChosen = 0x0906 UMETA(DisplayName = "SpecChosen"),                 // wire 0x2025
    SrvLearnedSpellsBatch = 0x0907 UMETA(DisplayName = "LearnedSpellsBatch"), // wire 0x2026
    CmdLfgLeave = 0x0908 UMETA(DisplayName = "LfgLeave"),                     // wire 0x2027
    CmdLfgProposalAccept = 0x0909 UMETA(DisplayName = "LfgProposalAccept"),   // wire 0x2028
    SrvLfgProposal = 0x090A UMETA(DisplayName = "LfgProposal"),               // wire 0x2029

    Max = 0xFFFF UMETA(Hidden),
};

/**
 * Маппинг клиентского блока-enum (EMMOOpcode) -> wire-номер из proto/opcodes.defs.
 * Единая точка правды для всех новых блоков (quests/gossip v1.3 и далее).
 */
namespace MMOWire {
inline constexpr uint16 ToWire(EMMOOpcode Op) {
    switch (Op) {
    // Quests (defs 0x2000..0x2007)
    case EMMOOpcode::CmdQuestQuery:
        return 0x2000;
    case EMMOOpcode::SrvQuestList:
        return 0x2001;
    case EMMOOpcode::CmdQuestAccept:
        return 0x2002;
    case EMMOOpcode::CmdQuestComplete:
        return 0x2003;
    case EMMOOpcode::SrvQuestAccepted:
        return 0x2004;
    case EMMOOpcode::SrvQuestProgress:
        return 0x2005;
    case EMMOOpcode::SrvQuestCompleteResult:
        return 0x2006;
    case EMMOOpcode::SrvQuestTurnInHint:
        return 0x2007;
    // Gossip (defs 0x2010..0x2014)
    case EMMOOpcode::CmdGossipHello:
        return 0x2010;
    case EMMOOpcode::SrvGossipShow:
        return 0x2011;
    case EMMOOpcode::CmdGossipSelect:
        return 0x2012;
    case EMMOOpcode::SrvGossipReply:
        return 0x2013;
    case EMMOOpcode::SrvCreatureText:
        return 0x2014;
    // Classes (defs 0x0090..0x0093)
    case EMMOOpcode::CmdClassInfoQuery:
        return 0x0090;
    case EMMOOpcode::SrvClassInfo:
        return 0x0091;
    case EMMOOpcode::SrvInitializeStats:
        return 0x0092;
    case EMMOOpcode::SrvLearnSpell:
        return 0x0093;
    // Quests 6-10: Spec selection + LFG (defs 0x2020..0x2029)
    case EMMOOpcode::CmdJoinLfg:
        return 0x2020;
    case EMMOOpcode::SrvLfgQueueState:
        return 0x2021;
    case EMMOOpcode::SrvLfgRoleCheck:
        return 0x2022;
    case EMMOOpcode::SrvSpecSelectionOpen:
        return 0x2023;
    case EMMOOpcode::CmdChooseSpec:
        return 0x2024;
    case EMMOOpcode::SrvSpecChosen:
        return 0x2025;
    case EMMOOpcode::SrvLearnedSpellsBatch:
        return 0x2026;
    case EMMOOpcode::CmdLfgLeave:
        return 0x2027;
    case EMMOOpcode::CmdLfgProposalAccept:
        return 0x2028;
    case EMMOOpcode::SrvLfgProposal:
        return 0x2029;
    default:
        return static_cast<uint16>(Op);
    }
}

inline constexpr EMMOOpcode FromWire(uint16 Wire) {
    switch (Wire) {
    case 0x2000:
        return EMMOOpcode::CmdQuestQuery;
    case 0x2001:
        return EMMOOpcode::SrvQuestList;
    case 0x2002:
        return EMMOOpcode::CmdQuestAccept;
    case 0x2003:
        return EMMOOpcode::CmdQuestComplete;
    case 0x2004:
        return EMMOOpcode::SrvQuestAccepted;
    case 0x2005:
        return EMMOOpcode::SrvQuestProgress;
    case 0x2006:
        return EMMOOpcode::SrvQuestCompleteResult;
    case 0x2007:
        return EMMOOpcode::SrvQuestTurnInHint;
    case 0x2010:
        return EMMOOpcode::CmdGossipHello;
    case 0x2011:
        return EMMOOpcode::SrvGossipShow;
    case 0x2012:
        return EMMOOpcode::CmdGossipSelect;
    case 0x2013:
        return EMMOOpcode::SrvGossipReply;
    case 0x2014:
        return EMMOOpcode::SrvCreatureText;
    case 0x0090:
        return EMMOOpcode::CmdClassInfoQuery;
    case 0x0091:
        return EMMOOpcode::SrvClassInfo;
    case 0x0092:
        return EMMOOpcode::SrvInitializeStats;
    case 0x0093:
        return EMMOOpcode::SrvLearnSpell;
    // Quests 6-10: Spec selection + LFG (defs 0x2020..0x2029)
    case 0x2020:
        return EMMOOpcode::CmdJoinLfg;
    case 0x2021:
        return EMMOOpcode::SrvLfgQueueState;
    case 0x2022:
        return EMMOOpcode::SrvLfgRoleCheck;
    case 0x2023:
        return EMMOOpcode::SrvSpecSelectionOpen;
    case 0x2024:
        return EMMOOpcode::CmdChooseSpec;
    case 0x2025:
        return EMMOOpcode::SrvSpecChosen;
    case 0x2026:
        return EMMOOpcode::SrvLearnedSpellsBatch;
    case 0x2027:
        return EMMOOpcode::CmdLfgLeave;
    case 0x2028:
        return EMMOOpcode::CmdLfgProposalAccept;
    case 0x2029:
        return EMMOOpcode::SrvLfgProposal;
    default:
        return static_cast<EMMOOpcode>(Wire);
    }
}
} // namespace MMOWire

/** Статусы авторизации (зеркало AuthResults из TRC). */
UENUM(BlueprintType)
enum class EMMOAuthStatus : uint8 {
    Success = 0 UMETA(DisplayName = "Success"),
    UnknownAccount = 1 UMETA(DisplayName = "UnknownAccount"),
    BadPassword = 2 UMETA(DisplayName = "BadPassword"),
    Failed = 3 UMETA(DisplayName = "ConnectionFailed"),
    AlreadyOnline = 4 UMETA(DisplayName = "AlreadyOnline"),
    Banned = 7 UMETA(DisplayName = "Banned"),
    WrongPass = 14 UMETA(DisplayName = "WrongPass"),
    SessionExpired = 15 UMETA(DisplayName = "SessionExpired"),
};

/** Состояние TCP-соединения клиента. */
UENUM(BlueprintType)
enum class EMMONetConnectionState : uint8 {
    Disconnected,
    Connecting,
    Connected, // транспорт установлен, но ещё не аутентифицирован
    Authenticating,
    Authenticated,
    Failing, // graceful shutdown initiated
};

/**
 * Лёгкий биначный пакет: заголовок + payload. Не UObject для скорости;
 * для Blueprint-экспозиции используется обёртка FMMOPacketBP (см. MMOGameInstance).
 */
struct FMMOPacket {
    uint16 Opcode = 0;
    TArray<uint8> Payload;

    FMMOPacket() = default;
    FMMOPacket(EMMOOpcode InOpcode, TArray<uint8> &&InPayload)
        : Opcode(static_cast<uint16>(InOpcode)), Payload(MoveTemp(InPayload)) {}

    // ---------- Serialize ----------
    /** Пишит фрейм [Size|Opcode|Payload] в OutBytes (LE). Возвращает false при переполнении лимита.
     */
    bool Serialize(TArray<uint8> &OutBytes) const {
        const uint32 TotalSize = MMOCORE_HEADER_SIZE + Payload.Num();
        if (TotalSize > MMOCORE_MAX_PACKET_SIZE) {
            return false;
        }
        OutBytes.Reset(TotalSize);
        OutBytes.SetNumUninitialized(TotalSize);

        uint8 *D = OutBytes.GetData();
        // Size (LE)
        D[0] = static_cast<uint8>(TotalSize & 0xFF);
        D[1] = static_cast<uint8>((TotalSize >> 8) & 0xFF);
        // Opcode (LE)
        D[2] = static_cast<uint8>(Opcode & 0xFF);
        D[3] = static_cast<uint8>((Opcode >> 8) & 0xFF);
        if (Payload.Num() > 0) {
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
    static bool TryDeserialize(const uint8 *Data, int32 Num, FMMOPacket &OutPacket,
                               int32 &OutConsumed) {
        if (Num < MMOCORE_HEADER_SIZE) {
            return false;
        }
        const uint8 *S = Data;
        const uint32 FrameSize = static_cast<uint32>(S[0]) | (static_cast<uint32>(S[1]) << 8);
        if (FrameSize < MMOCORE_HEADER_SIZE || FrameSize > MMOCORE_MAX_PACKET_SIZE) {
            // поток повреждён — соединение подлежит разрыву
            return false;
        }
        if (static_cast<uint32>(Num) < FrameSize) {
            return false; // ждём остаток
        }
        OutPacket.Opcode = static_cast<uint16>(S[2]) | (static_cast<uint16>(S[3]) << 8);
        const int32 PayloadLen = static_cast<int32>(FrameSize) - MMOCORE_HEADER_SIZE;
        OutPacket.Payload.SetNumUninitialized(PayloadLen);
        if (PayloadLen > 0) {
            FMemory::Memcpy(OutPacket.Payload.GetData(), S + MMOCORE_HEADER_SIZE, PayloadLen);
        }
        OutConsumed = static_cast<int32>(FrameSize);
        return true;
    }

    // ---------- Payload writer/reader helpers (TRC BigNumber/LittleEndian style) ----------
    void WriteU8(uint8 V) {
        Payload.Add(V);
    }
    void WriteU16(uint16 V) {
        AppendLE(V);
    }
    void WriteU32(uint32 V) {
        AppendLE(V);
    }
    void WriteU64(uint64 V) {
        AppendLE(V);
    }

    void WriteString(const FString &S) {
        FTCHARToUTF8 Utf8(*S);
        WriteU32(Utf8.Length());
        Payload.Append(reinterpret_cast<const uint8 *>(Utf8.Get()), Utf8.Length());
    }

    /** Чтектор поверх payload; инкрементальные read'ы. */
    struct FReader {
        const uint8 *P;
        int32 N;
        int32 Pos = 0;
        FReader(const TArray<uint8> &InPayload) : P(InPayload.GetData()), N(InPayload.Num()) {}

        int32 Remaining() const {
            return N - Pos;
        }

        bool ReadU8(uint8 &Out) {
            if (N - Pos < 1)
                return false;
            Out = P[Pos++];
            return true;
        }
        bool ReadU16(uint16 &Out) {
            if (N - Pos < 2)
                return false;
            Out = ReadLE<uint16>();
            return true;
        }
        bool ReadU32(uint32 &Out) {
            if (N - Pos < 4)
                return false;
            Out = ReadLE<uint32>();
            return true;
        }
        bool ReadU64(uint64 &Out) {
            if (N - Pos < 8)
                return false;
            Out = ReadLE<uint64>();
            return true;
        }
        bool ReadString(FString &Out) {
            uint32 Len;
            if (!ReadU32(Len) || Len > static_cast<uint32>(N - Pos))
                return false;
            Out = FString(UTF8_TO_TCHAR(reinterpret_cast<const ANSICHAR *>(P + Pos)));
            Pos += Len;
            return true;
        }
        bool ReadBytes(TArray<uint8> &Out, int32 Count) {
            if (Count < 0 || N - Pos < Count)
                return false;
            Out.Append(P + Pos, Count);
            Pos += Count;
            return true;
        }

    private:
        template <typename T> T ReadLE() {
            T V = 0;
            for (uint32 i = 0; i < sizeof(T); ++i)
                V |= static_cast<T>(static_cast<T>(P[Pos + i]) << (8 * i));
            Pos += sizeof(T);
            return V;
        }
    };

private:
    template <typename T> void AppendLE(T V) {
        for (uint32 i = 0; i < sizeof(T); ++i) {
            Payload.Add(static_cast<uint8>((V >> (8 * i)) & 0xFF));
        }
    }
};

// =============================================================================
// Character creation packets — зеркало /proto/opcodes.defs @version 1.1
//   CMSG_CHAR_CREATE        (CmdCharCreate, wire 0x0012)
//   SMSG_CHAR_CREATE_SUCCESS(SrvCharCreateSuccess, wire 0x0013)
//   SMSG_CHAR_CREATE_ERROR  (SrvCharCreateError, wire 0x0017)
// Blob кастомизации канонизируется сервером: клиент шлёт FCharacterCustomization,
// сервер парсит -> валидирует/клампит -> нормализует -> кладёт в БД (mediumblob)
// и возвращает байт-в-байт тот же blob в preview_customization.
// =============================================================================

/** Ошибки создания (ECharCreateError, TRC-style char create result). */
UENUM(BlueprintType)
enum class EMMOCharCreateError : uint8 {
    None = 0,
    NameInvalid = 1,       // регэксп/длина/спецсимволы
    NameReserved = 2,      // стоп-лист / «зарезервировано GM»
    NameExists = 3,        // уник в characters.name
    FactionInvalid = 4,    // вне 1..3 или несовместима с расой/классом
    RaceClassMismatch = 5, // класс недоступен расе
    CustomizationBlob = 6, // битый/перекачанный blob (>512 B)
    PerCharLimit = 7,      // >=10 персонажей на аккаунт
    ServerInternal = 8,
};

/** Пакет CMSG_CHAR_CREATE. */
struct FMMOCharCreatePacket {
    FString Name;                    // <=24 UTF-8 байт после нормализации
    uint8 Race = 0;                  // dbc Races.dbc entry
    uint8 Class = 0;                 // Classes.dbc entry
    uint8 Gender = 0;                // 0=male 1=female
    uint8 Faction = 0;               // 1=DawnGuard 2=BlackMatter 3=ThresholdGuild
    TArray<uint8> CustomizationBlob; // сериализованный FCharacterCustomization (<=512 B)

    static constexpr uint16 WIRE_OPCODE = 0x0012; // defs: CMSG_CHAR_CREATE

    /** Лимит blob'а на проводе (байт). Единая константа для обоих пакетов. */
    static constexpr int32 MAX_BLOB_SIZE = 512;

    FMMOPacket ToPacket() const {
        FMMOPacket P;
        P.Opcode = static_cast<uint16>(EMMOOpcode::CmdCharCreate);
        P.WriteString(Name);
        P.WriteU8(Race);
        P.WriteU8(Class);
        P.WriteU8(Gender);
        P.WriteU8(Faction);
        P.WriteU16(static_cast<uint16>(CustomizationBlob.Num()));
        P.Payload.Append(CustomizationBlob);
        return P;
    }

    static bool FromPacket(const FMMOPacket &P, FMMOCharCreatePacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        if (!R.ReadString(Out.Name))
            return false;
        if (!R.ReadU8(Out.Race) || !R.ReadU8(Out.Class))
            return false;
        if (!R.ReadU8(Out.Gender) || !R.ReadU8(Out.Faction))
            return false;
        uint16 BlobLen = 0;
        if (!R.ReadU16(BlobLen) || BlobLen > FMMOCharCreatePacket::MAX_BLOB_SIZE)
            return false;
        return R.ReadBytes(Out.CustomizationBlob, BlobLen);
    }
};

/** Пакет SMSG_CHAR_CREATE_SUCCESS (сервер → клиент). */
struct FMMOCharCreateSuccessPacket {
    uint32 CharacterId = 0; // guid
    FString Name;
    uint8 Race = 0, Class = 0, Gender = 0, Faction = 0;
    uint16 StartMap = 0;
    int32 StartX = 0, StartY = 0, StartZ = 0; // Q16.16 fixed (как EntityFull в map-канале)
    int32 StartOrientationQ16 = 0;            // yaw, Q16.16 (единый формат с координатами)
    TArray<uint8> PreviewCustomization;       // нормализованный blob — рендер списка без БД

    static constexpr uint16 WIRE_OPCODE = 0x0013; // defs: SMSG_CHAR_CREATE_SUCCESS

    static bool FromPacket(const FMMOPacket &P, FMMOCharCreateSuccessPacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        if (!R.ReadU32(Out.CharacterId))
            return false;
        if (!R.ReadString(Out.Name))
            return false;
        if (!R.ReadU8(Out.Race) || !R.ReadU8(Out.Class))
            return false;
        if (!R.ReadU8(Out.Gender) || !R.ReadU8(Out.Faction))
            return false;
        if (!R.ReadU16(Out.StartMap))
            return false;
        int32 X, Y, Z, O;
        // подписанные 32-битные поля читаем через ReadU32 + реинтерпретацию (LE, двух's complement)
        if (!R.ReadU32(reinterpret_cast<uint32 &>(X)))
            return false;
        if (!R.ReadU32(reinterpret_cast<uint32 &>(Y)))
            return false;
        if (!R.ReadU32(reinterpret_cast<uint32 &>(Z)))
            return false;
        if (!R.ReadU32(reinterpret_cast<uint32 &>(O)))
            return false;
        Out.StartX = X;
        Out.StartY = Y;
        Out.StartZ = Z;
        Out.StartOrientationQ16 = O;
        uint16 BlobLen = 0;
        if (!R.ReadU16(BlobLen) || BlobLen > FMMOCharCreatePacket::MAX_BLOB_SIZE)
            return false;
        return R.ReadBytes(Out.PreviewCustomization, BlobLen);
    }
};

/** Пакет SMSG_CHAR_CREATE_ERROR. */
struct FMMOCharCreateErrorPacket {
    EMMOCharCreateError Reason = EMMOCharCreateError::ServerInternal;

    static constexpr uint16 WIRE_OPCODE = 0x0017; // defs: SMSG_CHAR_CREATE_ERROR

    static bool FromPacket(const FMMOPacket &P, FMMOCharCreateErrorPacket &Out) {
        uint8 Raw = 0;
        if (!FMMOPacket::FReader(P.Payload).ReadU8(Raw))
            return false;
        Out.Reason = static_cast<EMMOCharCreateError>(Raw);
        return true;
    }
};

// =============================================================================
// v1.4 — СИСТЕМА КЛАССОВ (промпт «Система классов», docs/09)
//   CMSG_CLASS_INFO_QUERY     (CmdClassInfoQuery, wire 0x0090)
//   SMSG_CLASS_INFO           (SrvClassInfo,       wire 0x0091)
//   SMSG_INITIALIZE_STATS     (SrvInitializeStats, wire 0x0092)
//   SMSG_LEARN_SPELL          (SrvLearnSpell,      wire 0x0093)
// Wire-payload = proto/opcodes.defs (единственный источник истины).
// =============================================================================

/** Тип ресурса класса (зеркало PlayerClassMgr::PowerType). */
UENUM(BlueprintType)
enum class EMMOPowerType : uint8 {
    None = 0 UMETA(DisplayName = "None"),
    Mana = 1 UMETA(DisplayName = "Mana"),
    Rage = 2 UMETA(DisplayName = "Rage"),
    Energy = 3 UMETA(DisplayName = "Energy"),
};

/** Причина изучения спелла (поле source в SMSG_LEARN_SPELL). */
UENUM(BlueprintType)
// WARNING: Source=3 is used for Spec abilities (Server-side EItemSpellSource::Spec,
// см. dm-server/src/game/Quests/QuestSystem_Level10.cpp: p.WriteU8(/*source=*/3)).
// Do not confuse with Quest — квестовые награды используют отдельный канал (SMSG_QUEST_COMPLETE).
enum class EMMOSpellSource : uint8 {
    ClassDefault = 0 UMETA(DisplayName = "ClassDefault"), // wire 0x0093 source=0
    LevelUp = 1 UMETA(DisplayName = "LevelUp"),           // wire 0x0093 source=1
    Racial = 2 UMETA(DisplayName = "Racial"),             // wire 0x0093 source=2
    Spec = 3 UMETA(DisplayName = "Spec"), // wire 0x2026 source=3 (золотая подсветка action bar 10с)
};

/** Запрос таблицы Race×Class для экрана создания (CMSG_CLASS_INFO_QUERY). */
struct FMMOClassInfoQueryPacket {
    uint8 ForRace = 0; // 0 = все расы (полный дамп доступности)

    static constexpr uint16 WIRE_OPCODE = 0x0090;

    FMMOPacket ToPacket() const {
        FMMOPacket P;
        P.Opcode = static_cast<uint16>(EMMOOpcode::CmdClassInfoQuery);
        P.WriteU8(ForRace);
        return P;
    }
};

/** Ответ сервера: активные class/spec перса + список разрешённых рас. */
struct FMMOClassInfoPacket {
    uint32 CharGuid = 0;        // 0 = превью для UI_CharacterCreate
    uint8 Class = 0;            // CLASS_*
    uint8 Spec = 0;             // SPEC_*
    TArray<uint8> AllowedRaces; // terminates by explicit count field on wire

    static constexpr uint16 WIRE_OPCODE = 0x0091;
    static constexpr int32 MAX_ALLOWED = 6;

    static bool FromPacket(const FMMOPacket &P, FMMOClassInfoPacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        if (!R.ReadU32(Out.CharGuid))
            return false;
        if (!R.ReadU8(Out.Class) || !R.ReadU8(Out.Spec))
            return false;
        uint8 Count = 0;
        if (!R.ReadU8(Count) || Count > MAX_ALLOWED)
            return false;
        Out.AllowedRaces.Reset();
        for (uint8 i = 0; i < Count; ++i) {
            uint8 Race = 0;
            if (!R.ReadU8(Race))
                return false;
            Out.AllowedRaces.Add(Race);
        }
        return true;
    }
};

/** Полный снимок базовых характеристик при входе в мир (SMSG_INITIALIZE_STATS). */
struct FMMOInitializeStatsPacket {
    uint32 UnitGuid = 0;
    uint32 HealthMax = 0, HealthCur = 0;
    EMMOPowerType PowerType = EMMOPowerType::None;
    uint32 PowerMax = 0, PowerCur = 0;
    uint32 Strength = 0, Agility = 0, Stamina = 0, Intellect = 0, Spirit = 0;
    uint32 Armor = 0, SpellPower = 0;

    static constexpr uint16 WIRE_OPCODE = 0x0092;

    static bool FromPacket(const FMMOPacket &P, FMMOInitializeStatsPacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        if (!R.ReadU32(Out.UnitGuid))
            return false;
        if (!R.ReadU32(Out.HealthMax) || !R.ReadU32(Out.HealthCur))
            return false;
        uint8 PT = 0;
        if (!R.ReadU8(PT))
            return false;
        Out.PowerType = static_cast<EMMOPowerType>(PT);
        if (!R.ReadU32(Out.PowerMax) || !R.ReadU32(Out.PowerCur))
            return false;
        if (!R.ReadU32(Out.Strength) || !R.ReadU32(Out.Agility) || !R.ReadU32(Out.Stamina))
            return false;
        if (!R.ReadU32(Out.Intellect) || !R.ReadU32(Out.Spirit))
            return false;
        if (!R.ReadU32(Out.Armor) || !R.ReadU32(Out.SpellPower))
            return false;
        return true;
    }
};

/** Изучение способности (SMSG_LEARN_SPELL) — рисует иконку в action bar. */
struct FMMOLearnSpellPacket {
    uint32 SpellId = 0;
    EMMOSpellSource Source = EMMOSpellSource::ClassDefault;

    static constexpr uint16 WIRE_OPCODE = 0x0093;

    static bool FromPacket(const FMMOPacket &P, FMMOLearnSpellPacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        if (!R.ReadU32(Out.SpellId))
            return false;
        uint8 Src = 0;
        if (!R.ReadU8(Src))
            return false;
        Out.Source = static_cast<EMMOSpellSource>(Src);
        return true;
    }
};

// =============================================================================
// Spec selection + LFG packets (v1.5) — зеркало /proto/opcodes.defs 0x2020..0x2029.
// Контракт: sql/upgrades/Insert_Quests_6_to_10.sql, QuestSystem_Level10.h,
// UI_SpecSelectionWidget / LFGQueueWidget.
// =============================================================================

/** Роли LFG (биты = wire-маски RequiredRolesMask; серверный enum LfgRoleFlags). */
UENUM(BlueprintType)
enum class EMMOLfgRole : uint8 {
    None = 0 UMETA(DisplayName = "Нет роли"),
    Tank = 1 UMETA(DisplayName = "Танк"),
    Healer = 2 UMETA(DisplayName = "Целитель"),
    Dps = 4 UMETA(DisplayName = "Урон"),
};

/** Состояние очереди LFG (SMSG_LFG_QUEUE_STATE.state). */
UENUM(BlueprintType)
enum class EMMOLfgQueueState : uint8 {
    None = 0 UMETA(DisplayName = "Вне очереди"),
    Queued = 1 UMETA(DisplayName = "В поиске"),
    Proposal = 2 UMETA(DisplayName = "Предложение группы"),
    InInstance = 3 UMETA(DisplayName = "В подземелье"),
};

/** SMSG_SPEC_SELECTION_OPEN — «открой окно выбора специализации». */
struct FMMOSpecSelectionOpenPacket {
    uint8 ClassId = 0;     // CLASS_* (фильтр DT_SpecPanel)
    TArray<uint8> SpecIds; // доступные спеки (<=4)

    static constexpr uint16 WIRE_OPCODE = 0x2023;
    static constexpr int32 MAX_SPECS = 4;

    static bool FromPacket(const FMMOPacket &P, FMMOSpecSelectionOpenPacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        uint8 Count = 0;
        if (!R.ReadU8(Out.ClassId))
            return false;
        if (!R.ReadU8(Count) || Count > MAX_SPECS)
            return false;
        Out.SpecIds.Reset();
        for (uint8 i = 0; i < Count; ++i) {
            uint32 Id = 0;
            if (!R.ReadU32(Id))
                return false;
            Out.SpecIds.Add(static_cast<uint8>(Id));
        }
        return true;
    }
};

/** CMSG_CHOOSE_SPEC — клиент отправляет выбранный SpecID (wire 0x2024). */
struct FMMOChooseSpecPacket {
    uint8 SpecId = 0;

    static constexpr uint16 WIRE_OPCODE = 0x2024;

    FMMOPacket ToPacket() const {
        FMMOPacket P;
        P.Opcode = static_cast<uint16>(EMMOOpcode::CmdChooseSpec);
        P.WriteU8(SpecId);
        return P;
    }
};

/** SMSG_SPEC_CHOSEN — подтверждение сервера + роль для LFG (wire 0x2025). */
struct FMMOSpecChosenPacket {
    uint8 SpecId = 0;
    uint8 LfgRoleMask = 0; // биты EMMOLfgRole (Танк/Хил/ДД в LFGQueueWidget)

    static constexpr uint16 WIRE_OPCODE = 0x2025;

    static bool FromPacket(const FMMOPacket &P, FMMOSpecChosenPacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        return R.ReadU8(Out.SpecId) && R.ReadU8(Out.LfgRoleMask);
    }
};

/** SMSG_LEARNED_SPELLS_BATCH — батч новых заклинаний спеки (wire 0x2026). */
struct FMMOLearnedSpellsBatchPacket {
    TArray<uint32> SpellIds; // <=16
    uint8 Source = 0;        // 3 = spec -> золотая подсветка action bar 10с

    static constexpr uint16 WIRE_OPCODE = 0x2026;
    static constexpr int32 MAX_SPELLS = 16;

    static bool FromPacket(const FMMOPacket &P, FMMOLearnedSpellsBatchPacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        uint8 Count = 0;
        if (!R.ReadU8(Count) || Count > MAX_SPELLS)
            return false;
        Out.SpellIds.Reset();
        for (uint8 i = 0; i < Count; ++i) {
            uint32 Id = 0;
            if (!R.ReadU32(Id))
                return false;
            Out.SpellIds.Add(Id);
        }
        return R.ReadU8(Out.Source);
    }
};

/** CMSG_JOIN_LFG (wire 0x2020) / SMSG_LFG_QUEUE_STATE (0x2021). */
struct FMMOJoinLfgPacket {
    uint16 DungeonId = 0;
    uint8 RoleFlags = 0; // маска EMMOLfgRole игрока

    static constexpr uint16 WIRE_OPCODE = 0x2020;

    FMMOPacket ToPacket() const {
        FMMOPacket P;
        P.Opcode = static_cast<uint16>(EMMOOpcode::CmdJoinLfg);
        P.WriteU16(DungeonId);
        P.WriteU8(RoleFlags);
        return P;
    }
};

struct FMMOLfgQueueStatePacket {
    EMMOLfgQueueState State = EMMOLfgQueueState::None;
    uint16 DungeonId = 0;
    uint32 EstWaitMs = 0;

    static constexpr uint16 WIRE_OPCODE = 0x2021;

    static bool FromPacket(const FMMOPacket &P, FMMOLfgQueueStatePacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        uint8 St = 0;
        if (!R.ReadU8(St))
            return false;
        Out.State = static_cast<EMMOLfgQueueState>(St);
        return R.ReadU16(Out.DungeonId) && R.ReadU32(Out.EstWaitMs);
    }
};

/** SMSG_LFG_ROLE_CHECK — проверка роли перед входом в данж (wire 0x2022). */
struct FMMOLfgRoleCheckPacket {
    uint16 DungeonId = 0;
    uint8 RequiredRoles = 0; // маска данжа (lfg_dungeons.RequiredRolesMask)
    uint8 Ok = 0;            // 1 = роль игрока принимается очередью

    static constexpr uint16 WIRE_OPCODE = 0x2022;

    static bool FromPacket(const FMMOPacket &P, FMMOLfgRoleCheckPacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        return R.ReadU16(Out.DungeonId) && R.ReadU8(Out.RequiredRoles) && R.ReadU8(Out.Ok);
    }
};

/** SMSG_LFG_PROPOSAL — группа собрана, ждём акцепта (wire 0x2029). */
struct FMMOLfgProposalPacket {
    uint16 DungeonId = 0;
    uint32 MapId = 0;
    uint64 LeaderGuid = 0;

    static constexpr uint16 WIRE_OPCODE = 0x2029;

    static bool FromPacket(const FMMOPacket &P, FMMOLfgProposalPacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        return R.ReadU16(Out.DungeonId) && R.ReadU32(Out.MapId) && R.ReadU64(Out.LeaderGuid);
    }
};

// ---------------------------------------------------------------------------
// Quests 6-10 (proto/opcodes.defs 0x2000..0x2007): кэш журнала для UI.
// ---------------------------------------------------------------------------

/** Состояние квеста (QuestState.state в defs). */
UENUM(BlueprintType)
enum class EMMOQuestState : uint8 {
    Available = 0 UMETA(DisplayName = "Доступен"),
    Active = 1 UMETA(DisplayName = "Активен"),
    Completable = 2 UMETA(DisplayName = "Готов к сдаче"),
    Done = 3 UMETA(DisplayName = "Завершён"),
};

/** SMSG_QUEST_LIST (wire 0x2001): arr<QuestState>{64}. */
struct FMMOQuestStateEntry {
    uint32 QuestId = 0;
    EMMOQuestState State = EMMOQuestState::Available;
    uint8 Progress = 0;
};

struct FMMOQuestListPacket {
    TArray<FMMOQuestStateEntry> Quests; // <=64

    static constexpr uint16 WIRE_OPCODE = 0x2001;
    static constexpr int32 MAX_ENTRIES = 64;

    static bool FromPacket(const FMMOPacket &P, FMMOQuestListPacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        Out.Quests.Reset();
        while (R.Remaining() >= 6 && Out.Quests.Num() < MAX_ENTRIES) {
            FMMOQuestStateEntry E;
            uint8 St = 0;
            if (!R.ReadU32(E.QuestId) || !R.ReadU8(St) || !R.ReadU8(E.Progress))
                return false;
            E.State = static_cast<EMMOQuestState>(St);
            Out.Quests.Add(E);
        }
        return true;
    }
};

/** SMSG_QUEST_ACCEPTED (wire 0x2004): id + title + objectives + flags. */
struct FMMOQuestAcceptedPacket {
    uint32 QuestId = 0;
    FString Title;
    FString Objectives;
    uint8 QuestFlags = 0;

    static constexpr uint16 WIRE_OPCODE = 0x2004;

    static bool FromPacket(const FMMOPacket &P, FMMOQuestAcceptedPacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        return R.ReadU32(Out.QuestId) && R.ReadString(Out.Title) && R.ReadString(Out.Objectives) &&
               R.ReadU8(Out.QuestFlags);
    }
};

/** SMSG_QUEST_PROGRESS (wire 0x2005). */
struct FMMOQuestProgressPacket {
    uint32 QuestId = 0;
    uint8 Progress = 0;

    static constexpr uint16 WIRE_OPCODE = 0x2005;

    static bool FromPacket(const FMMOPacket &P, FMMOQuestProgressPacket &Out) {
        FMMOPacket::FReader R(P.Payload);
        return R.ReadU32(Out.QuestId) && R.ReadU8(Out.Progress);
    }
};
