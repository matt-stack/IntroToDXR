#pragma once
// pbrt is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
// The pbrt source code is licensed under the Apache License, Version 2.0.
// SPDX: Apache-2.0

#ifndef PBRT_PARSER_H
#define PBRT_PARSER_H

//#include "pbrt.h"

//#include "paramdict.h"
//#include "check.h"
//#include "containers.h" // not sure I need containers
//#include "error.h"
//#include "pstd.h"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <span>
#include <optional>
#include <vector>

//#include <filesystem/path.h>


#define CHECK(x) (!(!(x) && (std::printf("Check failed: %s", #x), true)))

namespace PBR {
/*
    static filesystem::path searchDirectory;

    void SetSearchDirectory(std::string filename) {
        filesystem::path path(filename);
        if (!path.is_directory())
            path = path.parent_path();
        searchDirectory = path;
    }
    */

    // Wstring things:
    std::u16string UTF16FromUTF8(std::string str);

    std::wstring WStringFromU16String(std::u16string str);

    std::wstring WStringFromUTF8(std::string str);


    struct FileLoc {
        FileLoc() = default;
        FileLoc(std::string_view filename) : filename(filename) {}
        std::string ToString() const;

        std::string_view filename;
        int line = 1, column = 0;
    };

    class ParsedParameter {
    public:
        // ParsedParameter Public Methods
        ParsedParameter(PBR::FileLoc loc) : loc(loc) {}

        void AddFloat(float v);
        void AddInt(int i);
        void AddString(std::string_view str);
        void AddBool(bool v);

        std::string ToString() const;

        // ParsedParameter Public Members
        std::string type, name;
        PBR::FileLoc loc;
        std::vector<float> floats;
        std::vector<int> ints;
        std::vector<std::string> strings;
        std::vector<uint8_t> bools;
        mutable bool lookedUp = false;
       // mutable const RGBColorSpace* colorSpace = nullptr;
        bool mayBeUnused = false;
    };


    using ParsedParameterVector = std::vector<ParsedParameter*>;


    // ParserTarget Definition
    class ParserTarget {
    public:
        // ParserTarget Interface
        // because ParserTarget is abstract, BasicSceneBuilder needs to implement all of these
        // pure virtual fns, for now we only use shape

        virtual void Scale(float sx, float sy, float sz, PBR::FileLoc loc) = 0;

        virtual void Shape(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;

        virtual ~ParserTarget();
/*
        virtual void Option(const std::string& name, const std::string& value,
            PBR::FileLoc loc) = 0;

        virtual void Identity(PBR::FileLoc loc) = 0;
        virtual void Translate(float dx, float dy, float dz, PBR::FileLoc loc) = 0;
        virtual void Rotate(float angle, float ax, float ay, float az, PBR::FileLoc loc) = 0;
        virtual void LookAt(float ex, float ey, float ez, float lx, float ly, float lz,
            float ux, float uy, float uz, PBR::FileLoc loc) = 0;
        virtual void ConcatTransform(float transform[16], PBR::FileLoc loc) = 0;
        virtual void Transform(float transform[16], PBR::FileLoc loc) = 0;
        virtual void CoordinateSystem(const std::string&, PBR::FileLoc loc) = 0;
        virtual void CoordSysTransform(const std::string&, PBR::FileLoc loc) = 0;
        virtual void ActiveTransformAll(PBR::FileLoc loc) = 0;
        virtual void ActiveTransformEndTime(PBR::FileLoc loc) = 0;
        virtual void ActiveTransformStartTime(PBR::FileLoc loc) = 0;
        virtual void TransformTimes(float start, float end, PBR::FileLoc loc) = 0;

        virtual void ColorSpace(const std::string& n, PBR::FileLoc loc) = 0;
        virtual void PixelFilter(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;
        virtual void Film(const std::string& type, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;
        virtual void Accelerator(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;
        virtual void Integrator(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;
        virtual void Camera(const std::string&, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;
        virtual void MakeNamedMedium(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;
        virtual void MediumInterface(const std::string& insideName,
            const std::string& outsideName, PBR::FileLoc loc) = 0;
        virtual void Sampler(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;

        virtual void WorldBegin(PBR::FileLoc loc) = 0;
        virtual void AttributeBegin(PBR::FileLoc loc) = 0;
        virtual void AttributeEnd(PBR::FileLoc loc) = 0;
        virtual void Attribute(const std::string& target, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;
        virtual void Texture(const std::string& name, const std::string& type,
            const std::string& texname, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;
        virtual void Material(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;
        virtual void MakeNamedMaterial(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;
        virtual void NamedMaterial(const std::string& name, PBR::FileLoc loc) = 0;
        virtual void LightSource(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;
        virtual void AreaLightSource(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc) = 0;
        virtual void ReverseOrientation(PBR::FileLoc loc) = 0;
        virtual void ObjectBegin(const std::string& name, PBR::FileLoc loc) = 0;
        virtual void ObjectEnd(PBR::FileLoc loc) = 0;
        virtual void ObjectInstance(const std::string& name, PBR::FileLoc loc) = 0;

        virtual void EndOfFiles() = 0;
*/
    protected:
        // ParserTarget Protected Methods
        template <typename... Args>
        void ErrorExitDeferred(const char* fmt, Args &&...args) const {
            errorExit = true;
            Error(fmt, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void ErrorExitDeferred(const PBR::FileLoc* loc, const char* fmt, Args &&...args) const {
            errorExit = true;
            Error(loc, fmt, std::forward<Args>(args)...);
        }

        mutable bool errorExit = false;
    };

    // Scene Parsing Declarations
   // void ParseFiles(ParserTarget* target, std::span<const std::string> filenames);
    void ParseString(ParserTarget* target, std::string str);

    // Token Definition
    struct Token {
        Token() = default;
        Token(std::string_view token, PBR::FileLoc loc) : token(token), loc(loc) {}
        std::string ToString() const;
        std::string_view token;
        PBR::FileLoc loc;
    };

    // Tokenizer Definition
    class Tokenizer {
    public:
        // Tokenizer Public Methods
        Tokenizer(std::string str, std::string filename,
            std::function<void(const char*, const PBR::FileLoc*)> errorCallback);
#if defined(PBRT_HAVE_MMAP) || defined(PBRT_IS_WINDOWS)
        Tokenizer(void* ptr, size_t len, std::string filename,
            std::function<void(const char*, const PBR::FileLoc*)> errorCallback);
#endif
        ~Tokenizer();

        static std::unique_ptr<Tokenizer> CreateFromFile(
            const std::string& filename,
            std::function<void(const char*, const PBR::FileLoc*)> errorCallback);
        static std::unique_ptr<Tokenizer> CreateFromString(
            std::string str,
            std::function<void(const char*, const PBR::FileLoc*)> errorCallback);

        std::optional<Token> Next();

        // Just for parse().
        // TODO? Have a method to set this?
        PBR::FileLoc loc;

    private:
        // Tokenizer Private Methods
        void CheckUTF(const void* ptr, int len) const;

        int getChar() {
            if (pos == end)
                return EOF;
            int ch = *pos++;
            if (ch == '\n') {
                ++loc.line;
                loc.column = 0;
            }
            else
                ++loc.column;
            return ch;
        }
        void ungetChar() {
            --pos;
            if (*pos == '\n')
                // Don't worry about the column; we'll be going to the start of
                // the next line again shortly...
                --loc.line;
        }

        // Tokenizer Private Members
        // This function is called if there is an error during lexing.
        std::function<void(const char*, const PBR::FileLoc*)> errorCallback;

#if defined(PBRT_HAVE_MMAP) || defined(PBRT_IS_WINDOWS)
        // Scene files on disk are mapped into memory for lexing.  We need to
        // hold on to the starting pointer and total length so they can be
        // unmapped in the destructor.
        void* unmapPtr = nullptr;
        size_t unmapLength = 0;
#endif

        // If the input is stdin, then we copy everything until EOF into this
        // string and then start lexing.  This is a little wasteful (versus
        // tokenizing directly from stdin), but makes the implementation
        // simpler.
        std::string contents;

        size_t tokenizerMemory;

        // Pointers to the current position in the file and one past the end of
        // the file.
        const char* pos, * end;

        // If there are escaped characters in the string, we can't just return
        // a std::string_view into the mapped file. In that case, we handle the
        // escaped characters and return a std::string_view to sEscaped.  (And
        // thence, std::string_views from previous calls to Next() must be invalid
        // after a subsequent call, since we may reuse sEscaped.)
        std::string sEscaped;
    };

    // FormattingParserTarget Definition
    class FormattingParserTarget : public ParserTarget {
    public:
        FormattingParserTarget(bool toPly, bool upgrade) : toPly(toPly), upgrade(upgrade) {}
        ~FormattingParserTarget();

        void Option(const std::string& name, const std::string& value, PBR::FileLoc loc);
        void Identity(PBR::FileLoc loc);
        void Translate(float dx, float dy, float dz, PBR::FileLoc loc);
        void Rotate(float angle, float ax, float ay, float az, PBR::FileLoc loc);
        void Scale(float sx, float sy, float sz, PBR::FileLoc loc);
        void LookAt(float ex, float ey, float ez, float lx, float ly, float lz, float ux,
            float uy, float uz, PBR::FileLoc loc);
        void ConcatTransform(float transform[16], PBR::FileLoc loc);
        void Transform(float transform[16], PBR::FileLoc loc);
        void CoordinateSystem(const std::string&, PBR::FileLoc loc);
        void CoordSysTransform(const std::string&, PBR::FileLoc loc);
        void ActiveTransformAll(PBR::FileLoc loc);
        void ActiveTransformEndTime(PBR::FileLoc loc);
        void ActiveTransformStartTime(PBR::FileLoc loc);
        void TransformTimes(float start, float end, PBR::FileLoc loc);
        void TransformBegin(PBR::FileLoc loc);
        void TransformEnd(PBR::FileLoc loc);
        void ColorSpace(const std::string& n, PBR::FileLoc loc);
        void PixelFilter(const std::string& name, ParsedParameterVector params, PBR::FileLoc loc);
        void Film(const std::string& type, ParsedParameterVector params, PBR::FileLoc loc);
        void Sampler(const std::string& name, ParsedParameterVector params, PBR::FileLoc loc);
        void Accelerator(const std::string& name, ParsedParameterVector params, PBR::FileLoc loc);
        void Integrator(const std::string& name, ParsedParameterVector params, PBR::FileLoc loc);
        void Camera(const std::string&, ParsedParameterVector params, PBR::FileLoc loc);
        void MakeNamedMedium(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc);
        void MediumInterface(const std::string& insideName, const std::string& outsideName,
            PBR::FileLoc loc);
        void WorldBegin(PBR::FileLoc loc);
        void AttributeBegin(PBR::FileLoc loc);
        void AttributeEnd(PBR::FileLoc loc);
        void Attribute(const std::string& target, ParsedParameterVector params, PBR::FileLoc loc);
        void Texture(const std::string& name, const std::string& type,
            const std::string& texname, ParsedParameterVector params, PBR::FileLoc loc);
        void Material(const std::string& name, ParsedParameterVector params, PBR::FileLoc loc);
        void MakeNamedMaterial(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc);
        void NamedMaterial(const std::string& name, PBR::FileLoc loc);
        void LightSource(const std::string& name, ParsedParameterVector params, PBR::FileLoc loc);
        void AreaLightSource(const std::string& name, ParsedParameterVector params,
            PBR::FileLoc loc);
        void Shape(const std::string& name, ParsedParameterVector params, PBR::FileLoc loc);
        void ReverseOrientation(PBR::FileLoc loc);
        void ObjectBegin(const std::string& name, PBR::FileLoc loc);
        void ObjectEnd(PBR::FileLoc loc);
        void ObjectInstance(const std::string& name, PBR::FileLoc loc);

        void EndOfFiles();

        std::string indent(int extra = 0) const {
            return std::string(catIndentCount + 4 * extra, ' ');
        }

    private:
//        std::string upgradeMaterialIndex(const std::string& name, ParameterDictionary* dict,
//            PBR::FileLoc loc) const;
//        std::string upgradeMaterial(std::string* name, ParameterDictionary* dict,
//            PBR::FileLoc loc) const;

        int catIndentCount = 0;
        bool toPly, upgrade;
        std::map<std::string, std::string> definedTextures;
        std::map<std::string, std::string> definedNamedMaterials;
//       std::map<std::string, ParameterDictionary> namedMaterialDictionaries;
        std::map<std::string, std::string> definedObjectInstances;
    };

}  // namespace pbrt

#endif  // PBRT_PARSER_H

