#pragma once

// pbrt is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
// The pbrt source code is licensed under the Apache License, Version 2.0.
// SPDX: Apache-2.0

//#include "PBR_include/pbrt.h"

//#include "PBR_include/texture.h"
//#include <pbrt/util/containers.h> // dont know if I need this one? Can make a std version
//#include "PBR_include/error.h"
//#include "PBR_include/memory.h"
//#include "PBR_include/pstd.h"
//#include <PBR_include/spectrum.h>
//#include <PBR_include/vecmath.h>
//#include <PBR_include/parser.h>

#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "PBR_include/parser.h"

namespace PBR {

    struct RGBColorSpace {
        // empty
    };

    struct Allocator {};
    struct Spectrum {};

    using Point2f = std::tuple<float, float>;
    using Vector2f = std::tuple<float, float>;
    using Point3f = std::tuple<float, float, float>;
    using Vector3f = std::tuple<float, float, float>;
    using Normal3f = std::tuple<float, float, float>;
/*
    // ParsedParameter Definition
    class ParsedParameter {
    public:
        // ParsedParameter Public Methods
        ParsedParameter(FileLoc loc) : loc(loc) {}

        void AddFloat(float v);
        void AddInt(int i);
        void AddString(std::string_view str);
        void AddBool(bool v);

        std::string ToString() const;

        // ParsedParameter Public Members
        std::string type, name;
        FileLoc loc;
        std::vector<float> floats;
        std::vector<int> ints;
        std::vector<std::string> strings;
       std::vector<uint8_t> bools;
        mutable bool lookedUp = false;
//        mutable const RGBColorSpace* colorSpace = nullptr;
        bool mayBeUnused = false;
    };

    // ParsedParameterVector Definition
//    using ParsedParameterVector = InlinedVector<ParsedParameter*, 8>;
    using ParsedParameterVector = std::vector<ParsedParameter*>;
*/
    // ParameterType Definition
    enum class ParameterType {
        Boolean,
        Float,
        Integer,
        Point2f,
        Vector2f,
        Point3f,
        Vector3f,
        Normal3f,
        Spectrum,
        String,
        Texture
    };

    // SpectrumType Definition
    enum class SpectrumType { Illuminant, Albedo, Unbounded };

    inline std::string ToString(SpectrumType t) {
        switch (t) {
        case SpectrumType::Albedo:
            return "Albedo";
        case SpectrumType::Unbounded:
            return "Unbounded";
        case SpectrumType::Illuminant:
            return "Illuminant";
        default:
            return "Illuminant";
            //LOG_FATAL("Unhandled SpectrumType");
        }
    }

    // NamedTextures Definition
    struct NamedTextures {
        /*
        // mstack might need this later for textures
        std::map<std::string, FloatTexture> floatTextures;
        std::map<std::string, SpectrumTexture> albedoSpectrumTextures;
        std::map<std::string, SpectrumTexture> unboundedSpectrumTextures;
        std::map<std::string, SpectrumTexture> illuminantSpectrumTextures;
        */
    };

    template <ParameterType PT>
    struct ParameterTypeTraits {};

    // ParameterDictionary Definition
    class ParameterDictionary {
    public:
        // ParameterDictionary Public Methods
        ParameterDictionary() = default;
        ParameterDictionary(ParsedParameterVector params);
//        ParameterDictionary(ParsedParameterVector params, const RGBColorSpace* colorSpace);

//        ParameterDictionary(ParsedParameterVector params0,
//            const ParsedParameterVector& params1,
//            const RGBColorSpace* colorSpace);

        ParameterDictionary(ParsedParameterVector params0,
            const ParsedParameterVector& params1);


        std::string GetTexture(const std::string& name) const;

//        std::vector<RGB> GetRGBArray(const std::string& name) const;

        // For --upgrade only
 //       std::optional<RGB> GetOneRGB(const std::string& name) const;
        // Unfortunately, this is most easily done here...
        float UpgradeBlackbody(const std::string& name);
        void RemoveFloat(const std::string&);
        void RemoveInt(const std::string&);
        void RemoveBool(const std::string&);
        void RemovePoint2f(const std::string&);
        void RemoveVector2f(const std::string&);
        void RemovePoint3f(const std::string&);
        void RemoveVector3f(const std::string&);
        void RemoveNormal3f(const std::string&);
        void RemoveString(const std::string&);
        void RemoveTexture(const std::string&);
        void RemoveSpectrum(const std::string&);

        void RenameParameter(const std::string& before, const std::string& after);
        void RenameUsedTextures(const std::map<std::string, std::string>& m);

        const RGBColorSpace* ColorSpace() const { return colorSpace; }

        std::string ToParameterList(int indent = 0) const;
        std::string ToParameterDefinition(const std::string&) const;
        std::string ToString() const;

        const FileLoc* loc(const std::string&) const;

        const ParsedParameterVector& GetParameterVector() const { return params; }

        void FreeParameters();

        float GetOneFloat(const std::string& name, float def) const;
        int GetOneInt(const std::string& name, int def) const;
        bool GetOneBool(const std::string& name, bool def) const;
        std::string GetOneString(const std::string& name, const std::string& def) const;

        Point2f GetOnePoint2f(const std::string& name, Point2f def) const;
        Vector2f GetOneVector2f(const std::string& name, Vector2f def) const;
        Point3f GetOnePoint3f(const std::string& name, Point3f def) const;
        Vector3f GetOneVector3f(const std::string& name, Vector3f def) const;
        Normal3f GetOneNormal3f(const std::string& name, Normal3f def) const;

//        Spectrum GetOneSpectrum(const std::string& name, Spectrum def,
//            SpectrumType spectrumType, Allocator alloc) const;

        std::vector<float> GetFloatArray(const std::string& name) const;
        std::vector<int> GetIntArray(const std::string& name) const;
        std::vector<uint8_t> GetBoolArray(const std::string& name) const;

        std::vector<Point2f> GetPoint2fArray(const std::string& name) const;
        std::vector<Vector2f> GetVector2fArray(const std::string& name) const;
        std::vector<Point3f> GetPoint3fArray(const std::string& name) const;
        std::vector<Vector3f> GetVector3fArray(const std::string& name) const;
        std::vector<Normal3f> GetNormal3fArray(const std::string& name) const;
        std::vector<Spectrum> GetSpectrumArray(const std::string& name,
            SpectrumType spectrumType,
            Allocator alloc) const;
        std::vector<std::string> GetStringArray(const std::string& name) const;

        void ReportUnused() const;

    private:
        friend class TextureParameterDictionary;
        // ParameterDictionary Private Methods
        template <ParameterType PT>
        typename ParameterTypeTraits<PT>::ReturnType lookupSingle(
            const std::string& name,
            typename ParameterTypeTraits<PT>::ReturnType defaultValue) const;

        template <ParameterType PT>
        std::vector<typename ParameterTypeTraits<PT>::ReturnType> lookupArray(
            const std::string& name) const;

        template <typename ReturnType, typename G, typename C>
        std::vector<ReturnType> lookupArray(const std::string& name, ParameterType type,
            const char* typeName, int nPerItem, G getValues,
            C convert) const;

        std::vector<Spectrum> extractSpectrumArray(const ParsedParameter& param,
            SpectrumType spectrumType,
            Allocator alloc) const;

        void remove(const std::string& name, const char* typeName);
        void checkParameterTypes();
        static std::string ToParameterDefinition(const ParsedParameter* p, int indentCount);

        // ParameterDictionary Private Members
        ParsedParameterVector params;
        const RGBColorSpace* colorSpace = nullptr;
        int nOwnedParams;
    };

    // TextureParameterDictionary Definition
    class TextureParameterDictionary {
    public:
        // TextureParameterDictionary Public Methods
        TextureParameterDictionary(const ParameterDictionary* dict,
            const NamedTextures* textures);

        operator const ParameterDictionary& () const { return *dict; }

        float GetOneFloat(const std::string& name, float def) const;
        int GetOneInt(const std::string& name, int def) const;
        bool GetOneBool(const std::string& name, bool def) const;
        Point2f GetOnePoint2f(const std::string& name, Point2f def) const;
        Vector2f GetOneVector2f(const std::string& name, Vector2f def) const;
        Point3f GetOnePoint3f(const std::string& name, Point3f def) const;
        Vector3f GetOneVector3f(const std::string& name, Vector3f def) const;
        Normal3f GetOneNormal3f(const std::string& name, Normal3f def) const;
        Spectrum GetOneSpectrum(const std::string& name, Spectrum def,
            SpectrumType spectrumType, Allocator alloc) const;
        std::string GetOneString(const std::string& name, const std::string& def) const;

        std::vector<float> GetFloatArray(const std::string& name) const;
        std::vector<int> GetIntArray(const std::string& name) const;
        std::vector<uint8_t> GetBoolArray(const std::string& name) const;
        std::vector<Point2f> GetPoint2fArray(const std::string& name) const;
        std::vector<Vector2f> GetVector2fArray(const std::string& name) const;
        std::vector<Point3f> GetPoint3fArray(const std::string& name) const;
        std::vector<Vector3f> GetVector3fArray(const std::string& name) const;
        std::vector<Normal3f> GetNormal3fArray(const std::string& name) const;
        std::vector<Spectrum> GetSpectrumArray(const std::string& name,
            SpectrumType spectrumType,
            Allocator alloc) const;
        std::vector<std::string> GetStringArray(const std::string& name) const;
/*
        FloatTexture GetFloatTexture(const std::string& name, Float defaultValue,
            Allocator alloc) const;
        FloatTexture GetFloatTextureOrNull(const std::string& name, Allocator alloc) const;

        void ReportUnused() const;

        SpectrumTexture GetSpectrumTexture(std::string name, Spectrum defaultValue,
            SpectrumType spectrumType, Allocator alloc) const;
        SpectrumTexture GetSpectrumTextureOrNull(std::string name, SpectrumType spectrumType,
            Allocator alloc) const;
*/
    private:
        // TextureParameterDictionary Private Members
        const ParameterDictionary* dict;
        const NamedTextures* textures;
    };

}  // namespace pbrt


