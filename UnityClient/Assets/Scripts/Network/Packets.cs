// Auto-generated from proto/opcodes.defs
using System;
using System.Collections.Generic;
using System.IO;

namespace DarkMatter.Network
{
    public class CharInfo
    {
        public uint id;
        public string name;
        public byte class;
        public byte faction;
        public byte level;
        public ushort map_id;
        public byte deleted;
        public static CharInfo FromReader(BinaryReader reader)
        {
            var o = new CharInfo();
            o.id = reader.ReadUInt32();
            o.name = reader.ReadString(24);
            o.class = reader.ReadByte();
            o.faction = reader.ReadByte();
            o.level = reader.ReadByte();
            o.map_id = reader.ReadUInt16();
            o.deleted = reader.ReadByte();
            return o;
        }
    }

    public class FrontlinePointState
    {
        public ushort point_id;
        public byte owner_faction;
        public ushort hold_progress_pct_x10;
        public static FrontlinePointState FromReader(BinaryReader reader)
        {
            var o = new FrontlinePointState();
            o.point_id = reader.ReadUInt16();
            o.owner_faction = reader.ReadByte();
            o.hold_progress_pct_x10 = reader.ReadUInt16();
            return o;
        }
    }

    public class SeepageInfo
    {
        public ushort zone_id;
        public byte intensity;
        public uint ends_ms_lo;
        public static SeepageInfo FromReader(BinaryReader reader)
        {
            var o = new SeepageInfo();
            o.zone_id = reader.ReadUInt16();
            o.intensity = reader.ReadByte();
            o.ends_ms_lo = reader.ReadUInt32();
            return o;
        }
    }

    public class MoveSubTick
    {
        public object dx;
        public object dy;
        public object dz;
        public byte dir_x8;
        public byte flags;
        public static MoveSubTick FromReader(BinaryReader reader)
        {
            var o = new MoveSubTick();
            o.dx = null;
            o.dy = null;
            o.dz = null;
            o.dir_x8 = reader.ReadByte();
            o.flags = reader.ReadByte();
            return o;
        }
    }

    public class EntityFull
    {
        public uint id;
        public byte type;
        public ushort entry;
        public int x;
        public ushort anim_state;
        public byte faction;
        public ushort hp_pct_x100;
        public uint auras_crc;
        public static EntityFull FromReader(BinaryReader reader)
        {
            var o = new EntityFull();
            o.id = reader.ReadUInt32();
            o.type = reader.ReadByte();
            o.entry = reader.ReadUInt16();
            o.x = reader.ReadInt32();
            o.anim_state = reader.ReadUInt16();
            o.faction = reader.ReadByte();
            o.hp_pct_x100 = reader.ReadUInt16();
            o.auras_crc = reader.ReadUInt32();
            return o;
        }
    }

    public class EntityDelta
    {
        public uint id;
        public byte fields_mask;
        public static EntityDelta FromReader(BinaryReader reader)
        {
            var o = new EntityDelta();
            o.id = reader.ReadUInt32();
            o.fields_mask = reader.ReadByte();
            return o;
        }
    }

    public class DamageUnit
    {
        public uint victim;
        public int amount_fixed;
        public byte school;
        public static DamageUnit FromReader(BinaryReader reader)
        {
            var o = new DamageUnit();
            o.victim = reader.ReadUInt32();
            o.amount_fixed = reader.ReadInt32();
            o.school = reader.ReadByte();
            return o;
        }
    }

    public class QuestState
    {
        public uint quest_id;
        public byte state;
        public byte progress;
        public static QuestState FromReader(BinaryReader reader)
        {
            var o = new QuestState();
            o.quest_id = reader.ReadUInt32();
            o.state = reader.ReadByte();
            o.progress = reader.ReadByte();
            return o;
        }
    }

    public class GossipTextEntry
    {
        public uint text_id;
        public string text_ru;
        public static GossipTextEntry FromReader(BinaryReader reader)
        {
            var o = new GossipTextEntry();
            o.text_id = reader.ReadUInt32();
            o.text_ru = reader.ReadString(256);
            return o;
        }
    }

    public class GossipOption
    {
        public byte option_id;
        public byte icon;
        public ushort text_broadcast_id;
        public byte has_quest;
        public static GossipOption FromReader(BinaryReader reader)
        {
            var o = new GossipOption();
            o.option_id = reader.ReadByte();
            o.icon = reader.ReadByte();
            o.text_broadcast_id = reader.ReadUInt16();
            o.has_quest = reader.ReadByte();
            return o;
        }
    }

    public class SMSG_AUTH_CHALLENGE
    {
        public const ushort Opcode = 0x0002;
        public byte[] salt_B;
        public byte[] pubkey_N_g;
        public static SMSG_AUTH_CHALLENGE FromBytes(byte[] data)
        {
            var o = new SMSG_AUTH_CHALLENGE();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.salt_B = reader.ReadBytes(128);
                o.pubkey_N_g = reader.ReadBytes(128);
            }
            return o;
        }
    }

    public class SMSG_AUTH_RESULT
    {
        public const ushort Opcode = 0x0004;
        public byte ok;
        public byte[] M2;
        public string realm_ticket;
        public static SMSG_AUTH_RESULT FromBytes(byte[] data)
        {
            var o = new SMSG_AUTH_RESULT();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.ok = reader.ReadByte();
                o.M2 = reader.ReadBytes(64);
                o.realm_ticket = reader.ReadString(40);
            }
            return o;
        }
    }

    public class SMSG_CHAR_LIST
    {
        public const ushort Opcode = 0x0011;
        public static SMSG_CHAR_LIST FromBytes(byte[] data)
        {
            var o = new SMSG_CHAR_LIST();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
            }
            return o;
        }
    }

    public class SMSG_CHAR_CREATE_SUCCESS
    {
        public const ushort Opcode = 0x0013;
        public uint id;
        public string name;
        public byte race;
        public byte class;
        public byte gender;
        public byte faction;
        public ushort start_map;
        public int start_x;
        public int start_o;
        public object preview_customization;
        public static SMSG_CHAR_CREATE_SUCCESS FromBytes(byte[] data)
        {
            var o = new SMSG_CHAR_CREATE_SUCCESS();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.id = reader.ReadUInt32();
                o.name = reader.ReadString(24);
                o.race = reader.ReadByte();
                o.class = reader.ReadByte();
                o.gender = reader.ReadByte();
                o.faction = reader.ReadByte();
                o.start_map = reader.ReadUInt16();
                o.start_x = reader.ReadInt32();
                o.start_o = reader.ReadInt32();
                o.preview_customization = null;
            }
            return o;
        }
    }

    public class SMSG_CHAR_CREATE_ERROR
    {
        public const ushort Opcode = 0x0017;
        public byte reason;
        public static SMSG_CHAR_CREATE_ERROR FromBytes(byte[] data)
        {
            var o = new SMSG_CHAR_CREATE_ERROR();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.reason = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_MAP_ASSIGN
    {
        public const ushort Opcode = 0x0015;
        public string ip;
        public ushort port;
        public byte[] map_session_key;
        public string reconnect_token;
        public ushort map_id;
        public static SMSG_MAP_ASSIGN FromBytes(byte[] data)
        {
            var o = new SMSG_MAP_ASSIGN();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.ip = reader.ReadString(48);
                o.port = reader.ReadUInt16();
                o.map_session_key = reader.ReadBytes(16);
                o.reconnect_token = reader.ReadString(40);
                o.map_id = reader.ReadUInt16();
            }
            return o;
        }
    }

    public class SMSG_CHAT_RECV
    {
        public const ushort Opcode = 0x0021;
        public byte channel;
        public uint sender;
        public string sender_name;
        public string text;
        public static SMSG_CHAT_RECV FromBytes(byte[] data)
        {
            var o = new SMSG_CHAT_RECV();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.channel = reader.ReadByte();
                o.sender = reader.ReadUInt32();
                o.sender_name = reader.ReadString(24);
                o.text = reader.ReadString(256);
            }
            return o;
        }
    }

    public class SMSG_REP_UPDATE
    {
        public const ushort Opcode = 0x0075;
        public byte faction;
        public int standing;
        public byte rank;
        public static SMSG_REP_UPDATE FromBytes(byte[] data)
        {
            var o = new SMSG_REP_UPDATE();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.faction = reader.ReadByte();
                o.standing = reader.ReadInt32();
                o.rank = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_RENEGADE_STATE
    {
        public const ushort Opcode = 0x0076;
        public byte is_renegade;
        public byte at_war_with_all;
        public ushort black_market_unlocked;
        public static SMSG_RENEGADE_STATE FromBytes(byte[] data)
        {
            var o = new SMSG_RENEGADE_STATE();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.is_renegade = reader.ReadByte();
                o.at_war_with_all = reader.ReadByte();
                o.black_market_unlocked = reader.ReadUInt16();
            }
            return o;
        }
    }

    public class SMSG_FACTION_VISUAL
    {
        public const ushort Opcode = 0x0077;
        public uint unit_guid;
        public byte faction;
        public byte apply_black_matter_fx;
        public static SMSG_FACTION_VISUAL FromBytes(byte[] data)
        {
            var o = new SMSG_FACTION_VISUAL();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.unit_guid = reader.ReadUInt32();
                o.faction = reader.ReadByte();
                o.apply_black_matter_fx = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_CREATURE_AGGRO
    {
        public const ushort Opcode = 0x0078;
        public uint creature_guid;
        public uint target_guid;
        public static SMSG_CREATURE_AGGRO FromBytes(byte[] data)
        {
            var o = new SMSG_CREATURE_AGGRO();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.creature_guid = reader.ReadUInt32();
                o.target_guid = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_FACTION_STATE
    {
        public const ushort Opcode = 0x0070;
        public byte season_active;
        public uint season_ends_ms_lo;
        public uint season_ends_ms_hi;
        public static SMSG_FACTION_STATE FromBytes(byte[] data)
        {
            var o = new SMSG_FACTION_STATE();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.season_active = reader.ReadByte();
                o.season_ends_ms_lo = reader.ReadUInt32();
                o.season_ends_ms_hi = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_SEEPAGE_LIST
    {
        public const ushort Opcode = 0x0073;
        public static SMSG_SEEPAGE_LIST FromBytes(byte[] data)
        {
            var o = new SMSG_SEEPAGE_LIST();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
            }
            return o;
        }
    }

    public class SMSG_CORRUPTION_STATE
    {
        public const ushort Opcode = 0x0074;
        public ushort value_x100;
        public byte altered;
        public static SMSG_CORRUPTION_STATE FromBytes(byte[] data)
        {
            var o = new SMSG_CORRUPTION_STATE();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.value_x100 = reader.ReadUInt16();
                o.altered = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_SNAPSHOT
    {
        public const ushort Opcode = 0x1010;
        public uint snapshot_seq;
        public uint server_time_ms_lo;
        public uint self_guid;
        public static SMSG_SNAPSHOT FromBytes(byte[] data)
        {
            var o = new SMSG_SNAPSHOT();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.snapshot_seq = reader.ReadUInt32();
                o.server_time_ms_lo = reader.ReadUInt32();
                o.self_guid = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_DELTA
    {
        public const ushort Opcode = 0x1011;
        public uint from_seq;
        public uint to_seq;
        public static SMSG_DELTA FromBytes(byte[] data)
        {
            var o = new SMSG_DELTA();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.from_seq = reader.ReadUInt32();
                o.to_seq = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_CAST_RESULT
    {
        public const ushort Opcode = 0x1022;
        public uint spell_id;
        public uint caster;
        public byte outcome;
        public ushort flags;
        public static SMSG_CAST_RESULT FromBytes(byte[] data)
        {
            var o = new SMSG_CAST_RESULT();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.spell_id = reader.ReadUInt32();
                o.caster = reader.ReadUInt32();
                o.outcome = reader.ReadByte();
                o.flags = reader.ReadUInt16();
            }
            return o;
        }
    }

    public class SMSG_SPAWN
    {
        public const ushort Opcode = 0x1040;
        public EntityFull entity;
        public static SMSG_SPAWN FromBytes(byte[] data)
        {
            var o = new SMSG_SPAWN();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.entity = EntityFull.FromReader(reader);
            }
            return o;
        }
    }

    public class SMSG_DESPAWN
    {
        public const ushort Opcode = 0x1041;
        public uint id;
        public static SMSG_DESPAWN FromBytes(byte[] data)
        {
            var o = new SMSG_DESPAWN();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.id = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_PONG
    {
        public const ushort Opcode = 0x1100;
        public uint client_time_ms_lo;
        public static SMSG_PONG FromBytes(byte[] data)
        {
            var o = new SMSG_PONG();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.client_time_ms_lo = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_QUEST_LIST
    {
        public const ushort Opcode = 0x2001;
        public static SMSG_QUEST_LIST FromBytes(byte[] data)
        {
            var o = new SMSG_QUEST_LIST();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
            }
            return o;
        }
    }

    public class SMSG_QUEST_ACCEPTED
    {
        public const ushort Opcode = 0x2004;
        public uint quest_id;
        public string title;
        public string objectives;
        public byte quest_flags;
        public static SMSG_QUEST_ACCEPTED FromBytes(byte[] data)
        {
            var o = new SMSG_QUEST_ACCEPTED();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.quest_id = reader.ReadUInt32();
                o.title = reader.ReadString(128);
                o.objectives = reader.ReadString(256);
                o.quest_flags = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_QUEST_PROGRESS
    {
        public const ushort Opcode = 0x2005;
        public uint quest_id;
        public static SMSG_QUEST_PROGRESS FromBytes(byte[] data)
        {
            var o = new SMSG_QUEST_PROGRESS();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.quest_id = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_QUEST_COMPLETE_RESULT
    {
        public const ushort Opcode = 0x2006;
        public uint quest_id;
        public byte result;
        public long reward_money;
        public static SMSG_QUEST_COMPLETE_RESULT FromBytes(byte[] data)
        {
            var o = new SMSG_QUEST_COMPLETE_RESULT();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.quest_id = reader.ReadUInt32();
                o.result = reader.ReadByte();
                o.reward_money = reader.ReadInt64();
            }
            return o;
        }
    }

    public class SMSG_QUEST_TURN_IN_HINT
    {
        public const ushort Opcode = 0x2007;
        public uint quest_id;
        public static SMSG_QUEST_TURN_IN_HINT FromBytes(byte[] data)
        {
            var o = new SMSG_QUEST_TURN_IN_HINT();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.quest_id = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_GOSSIP_SHOW
    {
        public const ushort Opcode = 0x2011;
        public uint npc_guid;
        public uint menu_id;
        public uint greeting_text_id;
        public static SMSG_GOSSIP_SHOW FromBytes(byte[] data)
        {
            var o = new SMSG_GOSSIP_SHOW();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.npc_guid = reader.ReadUInt32();
                o.menu_id = reader.ReadUInt32();
                o.greeting_text_id = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_GOSSIP_REPLY
    {
        public const ushort Opcode = 0x2013;
        public uint npc_guid;
        public uint text_id;
        public byte action;
        public uint payload_id;
        public static SMSG_GOSSIP_REPLY FromBytes(byte[] data)
        {
            var o = new SMSG_GOSSIP_REPLY();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.npc_guid = reader.ReadUInt32();
                o.text_id = reader.ReadUInt32();
                o.action = reader.ReadByte();
                o.payload_id = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_CREATURE_TEXT
    {
        public const ushort Opcode = 0x2014;
        public uint creature_guid;
        public uint text_id;
        public byte type;
        public static SMSG_CREATURE_TEXT FromBytes(byte[] data)
        {
            var o = new SMSG_CREATURE_TEXT();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.creature_guid = reader.ReadUInt32();
                o.text_id = reader.ReadUInt32();
                o.type = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_CLASS_INFO
    {
        public const ushort Opcode = 0x0091;
        public uint char_guid;
        public byte class;
        public byte spec;
        public static SMSG_CLASS_INFO FromBytes(byte[] data)
        {
            var o = new SMSG_CLASS_INFO();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.char_guid = reader.ReadUInt32();
                o.class = reader.ReadByte();
                o.spec = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_INITIALIZE_STATS
    {
        public const ushort Opcode = 0x0092;
        public uint unit;
        public uint health_max;
        public byte power_type;
        public uint power_max;
        public uint strength;
        public uint armor;
        public static SMSG_INITIALIZE_STATS FromBytes(byte[] data)
        {
            var o = new SMSG_INITIALIZE_STATS();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.unit = reader.ReadUInt32();
                o.health_max = reader.ReadUInt32();
                o.power_type = reader.ReadByte();
                o.power_max = reader.ReadUInt32();
                o.strength = reader.ReadUInt32();
                o.armor = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_LEARN_SPELL
    {
        public const ushort Opcode = 0x0093;
        public uint spell_id;
        public byte source;
        public static SMSG_LEARN_SPELL FromBytes(byte[] data)
        {
            var o = new SMSG_LEARN_SPELL();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.spell_id = reader.ReadUInt32();
                o.source = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_LFG_QUEUE_STATE
    {
        public const ushort Opcode = 0x2021;
        public byte state;
        public ushort dungeon_id;
        public uint est_wait_ms;
        public static SMSG_LFG_QUEUE_STATE FromBytes(byte[] data)
        {
            var o = new SMSG_LFG_QUEUE_STATE();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.state = reader.ReadByte();
                o.dungeon_id = reader.ReadUInt16();
                o.est_wait_ms = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_LFG_ROLE_CHECK
    {
        public const ushort Opcode = 0x2022;
        public ushort dungeon_id;
        public byte required_roles;
        public byte ok;
        public static SMSG_LFG_ROLE_CHECK FromBytes(byte[] data)
        {
            var o = new SMSG_LFG_ROLE_CHECK();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.dungeon_id = reader.ReadUInt16();
                o.required_roles = reader.ReadByte();
                o.ok = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_SPEC_SELECTION_OPEN
    {
        public const ushort Opcode = 0x2023;
        public byte class_id;
        public byte spec_count;
        public static SMSG_SPEC_SELECTION_OPEN FromBytes(byte[] data)
        {
            var o = new SMSG_SPEC_SELECTION_OPEN();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.class_id = reader.ReadByte();
                o.spec_count = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_SPEC_CHOSEN
    {
        public const ushort Opcode = 0x2025;
        public byte spec_id;
        public byte lfg_role_mask;
        public static SMSG_SPEC_CHOSEN FromBytes(byte[] data)
        {
            var o = new SMSG_SPEC_CHOSEN();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.spec_id = reader.ReadByte();
                o.lfg_role_mask = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_LEARNED_SPELLS_BATCH
    {
        public const ushort Opcode = 0x2026;
        public byte count;
        public static SMSG_LEARNED_SPELLS_BATCH FromBytes(byte[] data)
        {
            var o = new SMSG_LEARNED_SPELLS_BATCH();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.count = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_LFG_PROPOSAL
    {
        public const ushort Opcode = 0x2029;
        public ushort dungeon_id;
        public uint map_id;
        public ulong leader_guid;
        public static SMSG_LFG_PROPOSAL FromBytes(byte[] data)
        {
            var o = new SMSG_LFG_PROPOSAL();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.dungeon_id = reader.ReadUInt16();
                o.map_id = reader.ReadUInt32();
                o.leader_guid = reader.ReadUInt64();
            }
            return o;
        }
    }

    public class SMSG_LEVEL_UPDATE
    {
        public const ushort Opcode = 0x2030;
        public uint u8;
        public static SMSG_LEVEL_UPDATE FromBytes(byte[] data)
        {
            var o = new SMSG_LEVEL_UPDATE();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.u8 = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_XP_GAINED
    {
        public const ushort Opcode = 0x2031;
        public uint u32;
        public static SMSG_XP_GAINED FromBytes(byte[] data)
        {
            var o = new SMSG_XP_GAINED();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.u32 = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_TALENT_DATA
    {
        public const ushort Opcode = 0x2033;
        public ushort tree_count;
        public static SMSG_TALENT_DATA FromBytes(byte[] data)
        {
            var o = new SMSG_TALENT_DATA();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.tree_count = reader.ReadUInt16();
            }
            return o;
        }
    }

    public class SMSG_TALENT_RESULT
    {
        public const ushort Opcode = 0x2035;
        public uint talent_id;
        public static SMSG_TALENT_RESULT FromBytes(byte[] data)
        {
            var o = new SMSG_TALENT_RESULT();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.talent_id = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_ITEM_EQUIP_UPDATE
    {
        public const ushort Opcode = 0x2037;
        public uint u8;
        public static SMSG_ITEM_EQUIP_UPDATE FromBytes(byte[] data)
        {
            var o = new SMSG_ITEM_EQUIP_UPDATE();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.u8 = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_PVP_FLAG_UPDATE
    {
        public const ushort Opcode = 0x2041;
        public ulong guid;
        public ushort pvp_flags;
        public ushort flag_timer_sec;
        public byte wanted_level;
        public static SMSG_PVP_FLAG_UPDATE FromBytes(byte[] data)
        {
            var o = new SMSG_PVP_FLAG_UPDATE();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.guid = reader.ReadUInt64();
                o.pvp_flags = reader.ReadUInt16();
                o.flag_timer_sec = reader.ReadUInt16();
                o.wanted_level = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_PVP_ZONE_INFO
    {
        public const ushort Opcode = 0x2042;
        public ushort map_id;
        public ushort area_id;
        public byte ruleset;
        public byte forced_flag;
        public static SMSG_PVP_ZONE_INFO FromBytes(byte[] data)
        {
            var o = new SMSG_PVP_ZONE_INFO();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.map_id = reader.ReadUInt16();
                o.area_id = reader.ReadUInt16();
                o.ruleset = reader.ReadByte();
                o.forced_flag = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_DUEL_STARTED
    {
        public const ushort Opcode = 0x2044;
        public ulong duel_id;
        public static SMSG_DUEL_STARTED FromBytes(byte[] data)
        {
            var o = new SMSG_DUEL_STARTED();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.duel_id = reader.ReadUInt64();
            }
            return o;
        }
    }

    public class SMSG_DUEL_COMPLETE
    {
        public const ushort Opcode = 0x2046;
        public ulong duel_id;
        public static SMSG_DUEL_COMPLETE FromBytes(byte[] data)
        {
            var o = new SMSG_DUEL_COMPLETE();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.duel_id = reader.ReadUInt64();
            }
            return o;
        }
    }

    public class SMSG_PVP_KILL_NOTIFY
    {
        public const ushort Opcode = 0x2047;
        public ulong killer_guid;
        public ulong victim_guid;
        public ushort honor_gain;
        public int rep_delta;
        public byte dishonored;
        public static SMSG_PVP_KILL_NOTIFY FromBytes(byte[] data)
        {
            var o = new SMSG_PVP_KILL_NOTIFY();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.killer_guid = reader.ReadUInt64();
                o.victim_guid = reader.ReadUInt64();
                o.honor_gain = reader.ReadUInt16();
                o.rep_delta = reader.ReadInt32();
                o.dishonored = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_COMBAT_STATE
    {
        public const ushort Opcode = 0x2048;
        public uint u8;
        public static SMSG_COMBAT_STATE FromBytes(byte[] data)
        {
            var o = new SMSG_COMBAT_STATE();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.u8 = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_CORPSE_POLICY
    {
        public const ushort Opcode = 0x2049;
        public ulong guid;
        public byte mode;
        public ushort graveyard_map;
        public byte durability_loss_pct;
        public uint sickness_spell_id;
        public static SMSG_CORPSE_POLICY FromBytes(byte[] data)
        {
            var o = new SMSG_CORPSE_POLICY();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.guid = reader.ReadUInt64();
                o.mode = reader.ReadByte();
                o.graveyard_map = reader.ReadUInt16();
                o.durability_loss_pct = reader.ReadByte();
                o.sickness_spell_id = reader.ReadUInt32();
            }
            return o;
        }
    }

    public class SMSG_ZONE_MAP
    {
        public const ushort Opcode = 0x204B;
        public ushort zone_count;
        public ushort map_id;
        public ushort area_id;
        public byte category;
        public byte level_min;
        public byte level_max;
        public ushort faction_id;
        public byte ruleset;
        public static SMSG_ZONE_MAP FromBytes(byte[] data)
        {
            var o = new SMSG_ZONE_MAP();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.zone_count = reader.ReadUInt16();
                o.map_id = reader.ReadUInt16();
                o.area_id = reader.ReadUInt16();
                o.category = reader.ReadByte();
                o.level_min = reader.ReadByte();
                o.level_max = reader.ReadByte();
                o.faction_id = reader.ReadUInt16();
                o.ruleset = reader.ReadByte();
            }
            return o;
        }
    }

    public class SMSG_ZONE_UPDATE
    {
        public const ushort Opcode = 0x204C;
        public ushort map_id;
        public static SMSG_ZONE_UPDATE FromBytes(byte[] data)
        {
            var o = new SMSG_ZONE_UPDATE();
            using (var ms = new MemoryStream(data))
            using (var reader = new BinaryReader(ms))
            {
                o.map_id = reader.ReadUInt16();
            }
            return o;
        }
    }

}