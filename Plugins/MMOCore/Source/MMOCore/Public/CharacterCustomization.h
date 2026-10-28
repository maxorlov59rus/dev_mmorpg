// Copyright BlackMatter Team. All Rights Reserved.
//
// CharacterCustomization.h — каноническая модель кастомизации персонажа.
// Единый wire-формат (CustomizationBlob) для клиента (UE5.8) и сервера (dm-worldserver).
// Сервер ПЕРЕПАРСЫВАЕТ blob, клампит значения по CharacterCustomizationDef.json
// и возвращает нормализованные байты в SMSG_CHAR_CREATE_SUCCESS.PreviewCustomization.
//
// Wire layout (little-endian, порядок полей = docs + proto/opcodes.defs):
//   u16 BlobVersion (=1) | u8 MorphCount | MorphCount * { u16 SliderId; u16 ValueX1000 }
//   u8 HairStyle | u8 HairColorIdx | u8 EyeColorIdx | u8 SkinToneIdx
//   u8 ColorBGRA x4 ClothingPrimary | u8 ColorBGRA x4 ClothingSecondary | u16 Reserved(=0)
// Максимум морфов: 24 (лимит сервера). Макс blob: 512 B.

#pragma once

#include "CoreMinimal.h"
#include "CharacterCustomization.generated.h"

/** Индексы слайдеров лица (стабильные ID! Не переиспользовать после релиза). */
UENUM(BlueprintType)
enum class EFaceSlider : uint16 {
    JawWidth = 1,
    JawProjection = 2,
    CheekboneHeight = 3,
    CheekFullness = 4,
    NoseWidth = 5,
    NoseLength = 6,
    NoseBridgeHump = 7,
    EyeSpacing = 8,
    EyeSize = 9,
    EyeTilt = 10,
    BrowAngle = 11,
    BrowThickness = 12,
    LipFullness = 13,
    LipWidth = 14,
    EarSize = 15,
    ChinProjection = 16,
    ChinWidth = 17,
    ForeheadTilt = 18,
    Max = 18 UMETA(Hidden),
};

/** Слоты цвета: BGRA-упаковка (как FLinearColor.ToColor, но храним compactly). */
USTRUCT(BlueprintType)
struct FDMColorBGRA {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    uint8 B = 0, G = 0, R = 0, A = 255;

    FLinearColor ToLinear() const {
        return FLinearColor(R / 255.f, G / 255.f, B / 255.f, A / 255.f);
    }
    static FDMColorBGRA FromLinear(const FLinearColor &C) {
        auto Q = [](float V) {
            return static_cast<uint8>(FMath::Clamp(FMath::RoundToInt(V * 255.f), 0, 255));
        };
        return FDMColorBGRA{Q(C.B), Q(C.G), Q(C.R), 255};
    }
    bool operator==(const FDMColorBGRA &O) const {
        return B == O.B && G == O.G && R == O.R && A == O.A;
    }
};

/** Одна запись морфа. */
USTRUCT(BlueprintType)
struct FMMMorphValue {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    EFaceSlider Slider = EFaceSlider::JawWidth;

    /** Значение слайдера * 1000 (0..1000 => 0.0..1.0). Целочисленное — детерминированный wire. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    int32 ValueX1000 = 500;
};

/**
 * Полная кастомизация персонажа. Не UObject — дешёвая копия в стеке UI-треда.
 */
USTRUCT(BlueprintType)
struct FCharacterCustomization {
    GENERATED_BODY()

    static constexpr uint16 BLOB_VERSION = 1;
    static constexpr int32 MAX_MORPHS = 24;
    static constexpr int32 MAX_BLOB_BYTES = 512;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    TArray<FMMMorphValue> Morphs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization",
              meta = (ClampMin = "0", ClampMax = "15"))
    uint8 HairStyle = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization",
              meta = (ClampMin = "0", ClampMax = "15"))
    uint8 HairColorIdx = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization",
              meta = (ClampMin = "0", ClampMax = "15"))
    uint8 EyeColorIdx = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization",
              meta = (ClampMin = "0", ClampMax = "15"))
    uint8 SkinToneIdx = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    FDMColorBGRA ClothingPrimary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    FDMColorBGRA ClothingSecondary;

    // ---------------- Serialize / Deserialize (wire) ----------------

    bool Serialize(TArray<uint8> &OutBytes) const {
        if (Morphs.Num() > MAX_MORPHS)
            return false;
        OutBytes.Reset();
        auto W16 = [&](uint16 V) { OutBytes.Append(reinterpret_cast<const uint8 *>(&V), 2); };
        auto W8 = [&](uint8 V) { OutBytes.Add(V); };

        W16(BLOB_VERSION);
        W8(static_cast<uint8>(Morphs.Num()));
        for (const FMMMorphValue &M : Morphs) {
            W16(static_cast<uint16>(M.Slider));
            W16(static_cast<uint16>(FMath::Clamp(M.ValueX1000, 0, 1000)));
        }
        W8(HairStyle);
        W8(HairColorIdx);
        W8(EyeColorIdx);
        W8(SkinToneIdx);
        OutBytes.Append(reinterpret_cast<const uint8 *>(&ClothingPrimary), 4);
        OutBytes.Append(reinterpret_cast<const uint8 *>(&ClothingSecondary), 4);
        W16(0); // Reserved
        return OutBytes.Num() <= MAX_BLOB_BYTES;
    }

    static bool Deserialize(const TArray<uint8> &InBytes, FCharacterCustomization &Out) {
        if (InBytes.Num() < 2 || InBytes.Num() > MAX_BLOB_BYTES)
            return false;
        int32 Pos = 0;
        auto R8 = [&](uint8 &V) -> bool {
            if (Pos + 1 > InBytes.Num())
                return false;
            V = InBytes[Pos++];
            return true;
        };
        auto R16 = [&](uint16 &V) -> bool {
            if (Pos + 2 > InBytes.Num())
                return false;
            V = static_cast<uint16>(InBytes[Pos]) | (static_cast<uint16>(InBytes[Pos + 1]) << 8);
            Pos += 2;
            return true;
        };

        uint16 Version = 0;
        uint8 Count = 0;
        if (!R16(Version) || !R8(Count))
            return false;
        if (Version != BLOB_VERSION || Count > MAX_MORPHS)
            return false;

        Out.Morphs.Reset(Count);
        for (uint8 i = 0; i < Count; ++i) {
            uint16 Sid = 0, Val = 0;
            if (!R16(Sid) || !R16(Val))
                return false;
            if (Sid == 0 || Sid > static_cast<uint16>(EFaceSlider::Max))
                return false;
            Out.Morphs.Add(FMMMorphValue{static_cast<EFaceSlider>(Sid), static_cast<int32>(Val)});
        }
        if (!R8(Out.HairStyle) || !R8(Out.HairColorIdx))
            return false;
        if (!R8(Out.EyeColorIdx) || !R8(Out.SkinToneIdx))
            return false;
        if (Pos + 8 > InBytes.Num())
            return false;
        FMemory::Memcpy(&Out.ClothingPrimary, InBytes.GetData() + Pos, 4);
        Pos += 4;
        FMemory::Memcpy(&Out.ClothingSecondary, InBytes.GetData() + Pos, 4);
        Pos += 4;
        uint16 Reserved = 0;
        if (!R16(Reserved))
            return false;
        return true; // хвостовые байты допускаются (additive rule v2+)
    }

    /** Нормализация на клиенте перед отправкой: сортировка морфов по SliderId, дедупликация. */
    void Canonicalize() {
        Morphs.Sort(
            [](const FMMMorphValue &A, const FMMMorphValue &B) { return A.Slider < B.Slider; });
        for (int32 i = Morphs.Num() - 1; i > 0; --i) {
            if (Morphs[i].Slider == Morphs[i - 1].Slider)
                Morphs.RemoveAt(i);
        }
        for (FMMMorphValue &M : Morphs)
            M.ValueX1000 = FMath::Clamp(M.ValueX1000, 0, 1000);
        HairStyle = FMath::Min(HairStyle, static_cast<uint8>(15));
        HairColorIdx = FMath::Min(HairColorIdx, static_cast<uint8>(15));
        EyeColorIdx = FMath::Min(EyeColorIdx, static_cast<uint8>(15));
        SkinToneIdx = FMath::Min(SkinToneIdx, static_cast<uint8>(15));
    }

    /** Дефолт по расе/полу (зеркало CharacterCustomizationDef.json defaults). */
    static FCharacterCustomization MakeDefault(uint8 Race, uint8 Gender) {
        FCharacterCustomization C;
        C.HairStyle = (Gender == 0) ? 2 : 5;
        C.SkinToneIdx = static_cast<uint8>((Race + Gender) % 8);
        C.ClothingPrimary = FDMColorBGRA{40, 45, 60, 255};
        C.ClothingSecondary = FDMColorBGRA{120, 110, 90, 255};
        for (uint16 s = 1; s <= static_cast<uint16>(EFaceSlider::Max); ++s) {
            C.Morphs.Add(FMMMorphValue{static_cast<EFaceSlider>(s), 500});
        }
        return C;
    }
};
