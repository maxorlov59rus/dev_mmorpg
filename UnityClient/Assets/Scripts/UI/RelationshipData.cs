using UnityEngine;
using System;
using System.Collections.Generic;

namespace DM.Client
{
    public enum GenderType
    {
        Male = 0,
        Female = 1
    }

    public enum RelationshipType
    {
        Acquaintance = 0,
        Couple = 1,
        Engaged = 2,
        Married = 3
    }

    public enum FriendshipStatus
    {
        Pending = 0,
        Accepted = 1,
        Blocked = 2
    }

    public enum RelationshipStatus
    {
        Pending = 0,
        Active = 1,
        Dissolved = 2
    }

    public class FriendEntryData
    {
        public uint FriendGuid;
        public string FriendName;
        public FriendshipStatus Status;
        public uint CreatedAt;
        public uint AcceptedAt;
    }

    public class RelationshipEntryData
    {
        public uint Player1Guid;
        public uint Player2Guid;
        public string PartnerName;
        public RelationshipType Type;
        public RelationshipStatus Status;
        public uint CreatedAt;
        public uint AcceptedAt;
        public uint DissolvedAt;
    }

    public class RelationshipData
    {
        public static readonly string[] RelationshipTypeNamesRu = { "Знакомые", "Пара", "Помолвка", "Брак" };
        public static readonly string[] RelationshipTypeNamesEn = { "Acquaintance", "Couple", "Engaged", "Married" };
        public static readonly string[] GenderNamesRu = { "Мужской", "Женский" };
        public static readonly string[] GenderNamesEn = { "Male", "Female" };
    }
}
