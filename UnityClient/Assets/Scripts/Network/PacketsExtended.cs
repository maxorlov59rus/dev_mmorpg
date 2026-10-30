using System;
using System.IO;

namespace DarkMatter.Network
{
    public class CurrencyUpdatePacket
    {
        public byte currencyType;
        public uint amount;
        public static CurrencyUpdatePacket FromReader(BinaryReader reader)
        {
            var o = new CurrencyUpdatePacket();
            o.currencyType = reader.ReadByte();
            o.amount = reader.ReadUInt32();
            return o;
        }
    }

    public class CreateGuildResultPacket
    {
        public uint guildId;
        public byte result;
        public static CreateGuildResultPacket FromReader(BinaryReader reader)
        {
            var o = new CreateGuildResultPacket();
            o.guildId = reader.ReadUInt32();
            o.result = reader.ReadByte();
            return o;
        }
    }

    public class AuctionAddResultPacket
    {
        public uint auctionId;
        public byte result;
        public static AuctionAddResultPacket FromReader(BinaryReader reader)
        {
            var o = new AuctionAddResultPacket();
            o.auctionId = reader.ReadUInt32();
            o.result = reader.ReadByte();
            return o;
        }
    }

    public class AuctionBidResultPacket
    {
        public uint auctionId;
        public byte result;
        public uint highestBid;
        public static AuctionBidResultPacket FromReader(BinaryReader reader)
        {
            var o = new AuctionBidResultPacket();
            o.auctionId = reader.ReadUInt32();
            o.result = reader.ReadByte();
            o.highestBid = reader.ReadUInt32();
            return o;
        }
    }

    public class AuctionBuyoutResultPacket
    {
        public uint auctionId;
        public byte result;
        public static AuctionBuyoutResultPacket FromReader(BinaryReader reader)
        {
            var o = new AuctionBuyoutResultPacket();
            o.auctionId = reader.ReadUInt32();
            o.result = reader.ReadByte();
            return o;
        }
    }

    public class CraftResultPacket
    {
        public uint recipeId;
        public byte result;
        public static CraftResultPacket FromReader(BinaryReader reader)
        {
            var o = new CraftResultPacket();
            o.recipeId = reader.ReadUInt32();
            o.result = reader.ReadByte();
            return o;
        }
    }

    public class LearnRecipeResultPacket
    {
        public uint recipeId;
        public byte result;

        public static LearnRecipeResultPacket FromReader(BinaryReader reader)
        {
            var o = new LearnRecipeResultPacket();
            o.recipeId = reader.ReadUInt32();
            o.result = reader.ReadByte();
            return o;
        }
    }

    // --- Time System Packets ---

    /// <summary>
    /// SMSG_TIME_SYNC (0x2050): Sent by server to sync client game time.
    /// Format: hour(u8), minute(u8), day(u16), month(u8), year(u16), weatherType(u8), weatherSeverity(u8)
    /// </summary>
    public class TimeSyncPacket
    {
        public byte hour;
        public byte minute;
        public ushort day;
        public byte month;
        public ushort year;
        public byte weatherType;
        public byte weatherSeverity;
        public const ushort Opcode = 0x2050;

        public static TimeSyncPacket FromReader(BinaryReader reader)
        {
            var o = new TimeSyncPacket();
            o.hour = reader.ReadByte();
            o.minute = reader.ReadByte();
            o.day = reader.ReadUInt16();
            o.month = reader.ReadByte();
            o.year = reader.ReadUInt16();
            o.weatherType = reader.ReadByte();
            o.weatherSeverity = reader.ReadByte();
            return o;
        }
    }

    /// <summary>
    /// SMSG_WEATHER_UPDATE (0x2051): Sent by server to update current weather conditions.
    /// Format: weatherType(u8), severity(u8), duration(u32 ms), transition(u8 = 0 instant, 1 gradual)
    /// </summary>
    public class WeatherUpdatePacket
    {
        public byte weatherType;
        public byte severity;
        public uint durationMs;
        public byte transition;
        public const ushort Opcode = 0x2051;

        public static WeatherUpdatePacket FromReader(BinaryReader reader)
        {
            var o = new WeatherUpdatePacket();
            o.weatherType = reader.ReadByte();
            o.severity = reader.ReadByte();
            o.durationMs = reader.ReadUInt32();
            o.transition = reader.ReadByte();
            return o;
        }
    }


    // --- Resurrection System Packets ---

    /// <summary>
    /// SMSG_DEATH (0x1050): Sent by server when player dies.
    /// Format: corpseX(f32), corpseY(f32), corpseZ(f32), mapId(u16), inInstance(u8), inSeepage(u8)
    /// </summary>
    public class DeathPacket
    {
        public float corpseX;
        public float corpseY;
        public float corpseZ;
        public ushort mapId;
        public byte inInstance;
        public byte inSeepage;
        public const ushort Opcode = 0x1050;

        public static DeathPacket FromReader(BinaryReader reader)
        {
            var o = new DeathPacket();
            o.corpseX = reader.ReadSingle();
            o.corpseY = reader.ReadSingle();
            o.corpseZ = reader.ReadSingle();
            o.mapId = reader.ReadUInt16();
            o.inInstance = reader.ReadByte();
            o.inSeepage = reader.ReadByte();
            return o;
        }
    }

    /// <summary>
    /// SMSG_RESURRECT (0x1051): Sent by server to resurrect player.
    /// Format: type(u8), x(f32), y(f32), z(f32), mapId(u16), healthPct(u8), hasSickness(u8)
    /// </summary>
    public class ResurrectPacket
    {
        public byte type;
        public float x;
        public float y;
        public float z;
        public ushort mapId;
        public byte healthPct;
        public byte hasSickness;
        public const ushort Opcode = 0x1051;

        public static ResurrectPacket FromReader(BinaryReader reader)
        {
            var o = new ResurrectPacket();
            o.type = reader.ReadByte();
            o.x = reader.ReadSingle();
            o.y = reader.ReadSingle();
            o.z = reader.ReadSingle();
            o.mapId = reader.ReadUInt16();
            o.healthPct = reader.ReadByte();
            o.hasSickness = reader.ReadByte();
            return o;
        }
    }
}
