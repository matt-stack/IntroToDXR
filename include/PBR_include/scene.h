#pragma once

// pbrt is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
// The pbrt source code is licensed under the Apache License, Version 2.0.
// SPDX: Apache-2.0

//#ifndef PBRT_SCENE_H
//#define PBRT_SCENE_H
/*
#include <pbrt/pbrt.h>

#include <pbrt/cameras.h>
#include <pbrt/cpu/primitive.h>
#include <pbrt/paramdict.h>
#include <pbrt/parser.h>
#include <pbrt/util/containers.h>
#include <pbrt/util/error.h>
#include <pbrt/util/memory.h>
#include <pbrt/util/parallel.h>
#include <pbrt/util/print.h>
#include <pbrt/util/string.h>
#include <pbrt/util/transform.h>
*/

#include <PBR_include/parser.h>
#include <PBR_include/paramdict.h>
#include <PBR_include/transform.h>
#include <PBR_include/mesh.h>

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <cassert>

namespace PBR {

#define CHECK_IMPL(a, b, op) assert((a)op(b))

#define CHECK_EQ(a, b) CHECK_IMPL(a, b, ==)

    class Integrator;

    struct AnimatedTransform {
        // empty
    };
/*
    struct InstanceDefinitionSceneEntity {
        std::string name;
        FileLoc loc;
        // empty

        InstanceDefinitionSceneEntity(std::string n, FileLoc f) : name(n), loc(f) {};
    };
    */
    struct CameraSceneEntity {
        // empty
    };

    // placeholder
    struct Light {};

    // placeholder
    struct Material {};

    // placeholder
    struct FloatTexture {}; 

    // placeholder
    //struct Transform {};

    // SceneEntity Definition
    struct SceneEntity {
        // SceneEntity Public Methods
        SceneEntity() = default;
        SceneEntity(const std::string& name, ParameterDictionary parameters, FileLoc loc)
            : name(name), parameters(parameters), loc(loc) {}
            //: name(internedStrings.Lookup(name)), parameters(parameters), loc(loc) {}

        std::string ToString() const {
            return std::string("[ SceneEntity name: %s parameters: %s loc: %s ]");
                //, name,
               // parameters, loc);
        }

        // SceneEntity Public Members
        std::string name;
        FileLoc loc;
        ParameterDictionary parameters;
        static std::vector<std::string> internedStrings;
    };

    struct TransformedSceneEntity : public SceneEntity {
        TransformedSceneEntity() = default;
        TransformedSceneEntity(const std::string& name, ParameterDictionary parameters,
            FileLoc loc, const AnimatedTransform& renderFromObject)
            : SceneEntity(name, parameters, loc), renderFromObject(renderFromObject) {}

        std::string ToString() const {
            return std::string("[ TransformedSeneEntity name: %s parameters: %s loc: %s "
                "renderFromObject: %s ]");
                //name, parameters, loc, renderFromObject);
        }

        AnimatedTransform renderFromObject;
    };
/*
    // CameraSceneEntity Definition
    struct CameraSceneEntity : public SceneEntity {
        // CameraSceneEntity Public Methods
        CameraSceneEntity() = default;
        CameraSceneEntity(const std::string& name, ParameterDictionary parameters,
            FileLoc loc, const CameraTransform& cameraTransform,
            const std::string& medium)
            : SceneEntity(name, parameters, loc),
            cameraTransform(cameraTransform),
            medium(medium) {}

        std::string ToString() const {
            return StringPrintf("[ CameraSeneEntity name: %s parameters: %s loc: %s "
                "cameraTransform: %s medium: %s ]",
                name, parameters, loc, cameraTransform, medium);
        }

        CameraTransform cameraTransform;
        std::string medium;
    };
*/
    struct ShapeSceneEntity : public SceneEntity {
        ShapeSceneEntity() = default;
        ShapeSceneEntity(const std::string& name, ParameterDictionary parameters, FileLoc loc,
            const Transform* renderFromObject, const Transform* objectFromRender,
            bool reverseOrientation, int materialIndex,
            const std::string& materialName, int lightIndex,
            const std::string& insideMedium, const std::string& outsideMedium)
            : SceneEntity(name, parameters, loc),
            renderFromObject(renderFromObject),
            objectFromRender(objectFromRender),
            reverseOrientation(reverseOrientation),
            materialIndex(materialIndex),
            materialName(materialName),
            lightIndex(lightIndex),
            insideMedium(insideMedium),
            outsideMedium(outsideMedium) {}

        std::string ToString() const {
            return std::string(
                "[ ShapeSeneEntity name: %s parameters: %s loc: %s "
                "renderFromObject: %s objectFromRender: %s reverseOrientation: %s "
                "materialIndex: %d materialName: %s lightIndex: %d "
                "insideMedium: %s outsideMedium: %s]");
//                name, parameters, loc, *renderFromObject, *objectFromRender,
//                reverseOrientation, materialIndex, materialName, lightIndex, insideMedium,
//                outsideMedium);
        }

        const Transform* renderFromObject = nullptr, * objectFromRender = nullptr;
        bool reverseOrientation = false;
        int materialIndex;  // one of these two...  std::variant?
        std::string materialName;
        int lightIndex = -1;
        std::string insideMedium, outsideMedium;
    };
/*
    struct AnimatedShapeSceneEntity : public TransformedSceneEntity {
        AnimatedShapeSceneEntity() = default;
        AnimatedShapeSceneEntity(const std::string& name, ParameterDictionary parameters,
            FileLoc loc, const AnimatedTransform& renderFromObject,
            const Transform* identity, bool reverseOrientation,
            int materialIndex, const std::string& materialName,
            int lightIndex, const std::string& insideMedium,
            const std::string& outsideMedium)
            : TransformedSceneEntity(name, parameters, loc, renderFromObject),
            identity(identity),
            reverseOrientation(reverseOrientation),
            materialIndex(materialIndex),
            materialName(materialName),
            lightIndex(lightIndex),
            insideMedium(insideMedium),
            outsideMedium(outsideMedium) {}

        std::string ToString() const {
            return StringPrintf(
                "[ AnimatedShapeSeneEntity name: %s parameters: %s loc: %s "
                "renderFromObject: %s reverseOrientation: %s materialIndex: %d "
                "materialName: %s insideMedium: %s outsideMedium: %s]",
                name, parameters, loc, renderFromObject, reverseOrientation, materialIndex,
                materialName, insideMedium, outsideMedium);
        }

        const Transform* identity = nullptr;
        bool reverseOrientation = false;
        int materialIndex;  // one of these two...  std::variant?
        std::string materialName;
        int lightIndex = -1;
        std::string insideMedium, outsideMedium;
    };
*/
    
    struct InstanceDefinitionSceneEntity {
        InstanceDefinitionSceneEntity() = default;
        InstanceDefinitionSceneEntity(const std::string& name, FileLoc loc)
            : name(name), loc(loc) {}

        std::string ToString() const {
            return std::string("[ InstanceDefinitionSceneEntity name: %s loc: %s shapes: %s animatedShapes: %s ]");
                //name, loc, shapes, animatedShapes);
        }

        std::string name;
        FileLoc loc;
        std::vector<ShapeSceneEntity> shapes;
        //std::vector<AnimatedShapeSceneEntity> animatedShapes;
    };

    using MediumSceneEntity = TransformedSceneEntity;
    using TextureSceneEntity = TransformedSceneEntity;

    struct LightSceneEntity : public TransformedSceneEntity {
        LightSceneEntity() = default;
        LightSceneEntity(const std::string& name, ParameterDictionary parameters, FileLoc loc,
            const AnimatedTransform& renderFromLight, const std::string& medium)
            : TransformedSceneEntity(name, parameters, loc, renderFromLight),
            medium(medium) {}

        std::string ToString() const {
            return std::string("[ LightSeneEntity name: %s parameters: %s loc: %s "
                "renderFromObject: %s medium: %s ]");
                //name, parameters, loc, renderFromObject, medium);
        }

        std::string medium;
    };

    struct InstanceSceneEntity {
        InstanceSceneEntity() = default;
        InstanceSceneEntity(const std::string& n, FileLoc loc,
            const AnimatedTransform& renderFromInstanceAnim)
            : name(n),
            loc(loc),
            renderFromInstanceAnim(new AnimatedTransform(renderFromInstanceAnim)) {
            //CHECK(this->renderFromInstanceAnim->IsAnimated());
        }
        InstanceSceneEntity(const std::string& n, FileLoc loc,
            const Transform* renderFromInstance)
            : name(n),
            loc(loc),
            renderFromInstance(renderFromInstance) {}

        std::string ToString() const {
            return std::string(
                "[ InstanceSeneEntity name: %s loc: %s "
                "renderFromInstanceAnim: %s renderFromInstance: %s ]");
                //name, loc,
                //renderFromInstanceAnim ? renderFromInstanceAnim->ToString()
                //: std::string("nullptr"),
                //renderFromInstance ? renderFromInstance->ToString() : std::string("nullptr"));
        }

        std::string name;
        FileLoc loc;
        AnimatedTransform* renderFromInstanceAnim = nullptr;
        const Transform* renderFromInstance = nullptr;
    };

    // MaxTransforms Definition
    constexpr int MaxTransforms = 2;

    // TransformSet Definition
    struct TransformSet {
        // TransformSet Public Methods
        Transform& operator[](int i) {
            //if (i < 0 && i > MaxTransforms) { return t[i]; };
            return t[i];
        }
        const Transform& operator[](int i) const {
            //CHECK_GE(i, 0);
            //CHECK_LT(i, MaxTransforms);
            return t[i];
        }
        friend TransformSet Inverse(const TransformSet& ts) {
            TransformSet tInv;
            for (int i = 0; i < MaxTransforms; ++i)
                tInv.t[i] = Inverse(ts.t[i]);
            return tInv;
        }
//        bool IsAnimated() const {
//            for (int i = 0; i < MaxTransforms - 1; ++i)
//                if (t[i] != t[i + 1])
//                    return true;
//            return false;
//        }
    
    private:
        Transform t[MaxTransforms];
    };

    // BasicScene Definition
    class BasicScene {
    public:
        // BasicScene Public Methods
        BasicScene();

        void SetOptions(SceneEntity filter, SceneEntity film, CameraSceneEntity camera,
            SceneEntity sampler, SceneEntity integrator, SceneEntity accelerator);

        void AddNamedMaterial(std::string name, SceneEntity material);
        int AddMaterial(SceneEntity material);
//        void AddMedium(MediumSceneEntity medium);
//        void AddFloatTexture(std::string name, TextureSceneEntity texture);
//        void AddSpectrumTexture(std::string name, TextureSceneEntity texture);
        void AddLight(LightSceneEntity light);
        int AddAreaLight(SceneEntity light);
        void AddShapes(std::vector<ShapeSceneEntity> shape);
//        void AddAnimatedShape(AnimatedShapeSceneEntity shape);
        void AddInstanceDefinition(InstanceDefinitionSceneEntity instance);
//        void AddInstanceUses(pstd::span<InstanceSceneEntity> in);

        void Done();
/*
        Camera GetCamera() {
            cameraJobMutex.lock();
            while (!camera) {
                pstd::optional<Camera> c = cameraJob->TryGetResult(&cameraJobMutex);
                if (c)
                    camera = *c;
            }
            cameraJobMutex.unlock();
            LOG_VERBOSE("Retrieved Camera from future");
            return camera;
        }

        Sampler GetSampler() {
            samplerJobMutex.lock();
            while (!sampler) {
                pstd::optional<Sampler> s = samplerJob->TryGetResult(&samplerJobMutex);
                if (s)
                    sampler = *s;
            }
            samplerJobMutex.unlock();
            LOG_VERBOSE("Retrieved Sampler from future");
            return sampler;
        }
*/
        void CreateMaterials(const NamedTextures& sceneTextures,
            std::map<std::string, Material>* namedMaterials,
            std::vector<Material>* materials);

        std::vector<Light> CreateLights(
            const NamedTextures& textures,
            std::map<int, std::vector<Light>*>* shapeIndexToAreaLights);

//        std::map<std::string, Medium> CreateMedia();
        //Primitive CreateAggregate( // dont think we need to out a primative (for accel)
        void CreateGeometry(
            const NamedTextures& textures,
            const std::map<std::string, Material>& namedMaterials,
            const std::vector<Material>& materials);

        // NEED CREATETEXTURE
            
/*
        std::unique_ptr<Integrator> CreateIntegrator(Camera camera, Sampler sampler,
            Primitive accel,
            std::vector<Light> lights) const;
            */
//        NamedTextures CreateTextures();

        // BasicScene Public Members
        SceneEntity integrator, accelerator;
//        const RGBColorSpace* filmColorSpace;
        std::vector<ShapeSceneEntity> shapes;
//        std::vector<AnimatedShapeSceneEntity> animatedShapes;
        std::vector<InstanceSceneEntity> instances;
//        std::map<InternedString, InstanceDefinitionSceneEntity*> instanceDefinitions;

    private:
        // BasicScene Private Methods
//        Medium GetMedium(const std::string& name, const FileLoc* loc);

        void startLoadingNormalMaps(const ParameterDictionary& parameters);

        // BasicScene Private Members
//        AsyncJob<Sampler>* samplerJob = nullptr;
//        mutable ThreadLocal<Allocator> threadAllocators;
//        Camera camera;
//        Film film;
//        std::mutex cameraJobMutex;
//        AsyncJob<Camera>* cameraJob = nullptr;
//        std::mutex samplerJobMutex;
//        Sampler sampler;
//        std::mutex mediaMutex;
//        std::map<std::string, AsyncJob<Medium>*> mediumJobs;
//        std::map<std::string, Medium> mediaMap;
//        std::mutex materialMutex;
//        std::map<std::string, AsyncJob<Image*>*> normalMapJobs;
//        std::map<std::string, Image*> normalMaps; // mstack will likely need this when we get normal maps

        std::vector<std::pair<std::string, SceneEntity>> namedMaterials;
        std::vector<SceneEntity> materials;

        std::mutex lightMutex;
//        std::vector<AsyncJob<Light>*> lightJobs;

        std::mutex areaLightMutex;
        std::vector<SceneEntity> areaLights;

        std::mutex textureMutex;
//        std::vector<std::pair<std::string, TextureSceneEntity>> serialFloatTextures;
//        std::vector<std::pair<std::string, TextureSceneEntity>> serialSpectrumTextures;
//        std::vector<std::pair<std::string, TextureSceneEntity>> asyncSpectrumTextures;
        std::set<std::string> loadingTextureFilenames;
//        std::map<std::string, AsyncJob<FloatTexture>*> floatTextureJobs;
//        std::map<std::string, AsyncJob<SpectrumTexture>*> spectrumTextureJobs;
        int nMissingTextures = 0;

        std::mutex shapeMutex, animatedShapeMutex;
        std::mutex instanceDefinitionMutex, instanceUseMutex;
    };

    // BasicSceneBuilder Definition
    class BasicSceneBuilder : public ParserTarget {
    public:
        // BasicSceneBuilder Public Methods
        BasicSceneBuilder(BasicScene* scene);
//        void Option(const std::string& name, const std::string& value, FileLoc loc);
        void Identity(FileLoc loc);
        void Translate(float dx, float dy, float dz, FileLoc loc);
        void Rotate(float angle, float ax, float ay, float az, FileLoc loc);
        void Scale(float sx, float sy, float sz, FileLoc loc);
        void LookAt(float ex, float ey, float ez, float lx, float ly, float lz, float ux,
            float uy, float uz, FileLoc loc);
        void ConcatTransform(float transform[16], FileLoc loc);
        void Transform(float transform[16], FileLoc loc);
        void CoordinateSystem(const std::string&, FileLoc loc);
        void CoordSysTransform(const std::string&, FileLoc loc);
        void ActiveTransformAll(FileLoc loc);
        void ActiveTransformEndTime(FileLoc loc);
        void ActiveTransformStartTime(FileLoc loc);
        void TransformTimes(float start, float end, FileLoc loc);
        void ColorSpace(const std::string& n, FileLoc loc);
        void PixelFilter(const std::string& name, ParsedParameterVector params, FileLoc loc);
        void Film(const std::string& type, ParsedParameterVector params, FileLoc loc);
        void Sampler(const std::string& name, ParsedParameterVector params, FileLoc loc);
        void Accelerator(const std::string& name, ParsedParameterVector params, FileLoc loc);
        void Integrator(const std::string& name, ParsedParameterVector params, FileLoc loc);
        void Camera(const std::string&, ParsedParameterVector params, FileLoc loc);
        void MakeNamedMedium(const std::string& name, ParsedParameterVector params,
            FileLoc loc);
        void MediumInterface(const std::string& insideName, const std::string& outsideName,
            FileLoc loc);
        void WorldBegin(FileLoc loc);
        void AttributeBegin(FileLoc loc);
        void AttributeEnd(FileLoc loc);
        void Attribute(const std::string& target, ParsedParameterVector params, FileLoc loc);
        void Texture(const std::string& name, const std::string& type,
            const std::string& texname, ParsedParameterVector params, FileLoc loc);
        void Material(const std::string& name, ParsedParameterVector params, FileLoc loc);
        void MakeNamedMaterial(const std::string& name, ParsedParameterVector params,
            FileLoc loc);
        void NamedMaterial(const std::string& name, FileLoc loc);
        void LightSource(const std::string& name, ParsedParameterVector params, FileLoc loc);
        void AreaLightSource(const std::string& name, ParsedParameterVector params,
            FileLoc loc);
        void Shape(const std::string& name, ParsedParameterVector params, FileLoc loc);
        void ReverseOrientation(FileLoc loc);
        void ObjectBegin(const std::string& name, FileLoc loc);
        void ObjectEnd(FileLoc loc);
        void ObjectInstance(const std::string& name, FileLoc loc);

        void EndOfFiles();

        BasicSceneBuilder* CopyForImport();
        void MergeImported(BasicSceneBuilder*);

        std::string ToString() const;

    private:
        // BasicSceneBuilder::GraphicsState Definition
        struct GraphicsState {
            // GraphicsState Public Methods
            GraphicsState();

            template <typename F>
            void ForActiveTransforms(F func) {
                for (int i = 0; i < MaxTransforms; ++i)
                    if (activeTransformBits & (1 << i))
                        ctm[i] = func(ctm[i]);
            }

            // GraphicsState Public Members
            std::string currentInsideMedium, currentOutsideMedium;

            int currentMaterialIndex = 0;
            std::string currentMaterialName;

            std::string areaLightName;
            ParameterDictionary areaLightParams;
            FileLoc areaLightLoc;

            ParsedParameterVector shapeAttributes;
            ParsedParameterVector lightAttributes;
            ParsedParameterVector materialAttributes;
            ParsedParameterVector mediumAttributes;
            ParsedParameterVector textureAttributes;
            bool reverseOrientation = false;
//            const RGBColorSpace* colorSpace;
            TransformSet ctm;
            uint32_t activeTransformBits = AllTransformsBits;
            float transformStartTime = 0, transformEndTime = 1;
        };

        friend void parse(ParserTarget* scene, std::unique_ptr<Tokenizer> t);
        // BasicSceneBuilder Private Methods
        
        // renderFromWorld can just be a default init Transform
        class Transform RenderFromObject(int index) const {
            return PBR::Transform((renderFromWorld * graphicsState.ctm[index]).GetMatrix());
        }

//        AnimatedTransform RenderFromObject() const {
//            return { RenderFromObject(0), graphicsState.transformStartTime,
//                    RenderFromObject(1), graphicsState.transformEndTime };
//        }

        /*
        bool CTMIsAnimated() const { return graphicsState.ctm.IsAnimated(); }
*/
        // BasicSceneBuilder Private Members
        BasicScene* scene;
        enum class BlockState { OptionsBlock, WorldBlock };
        BlockState currentBlock = BlockState::OptionsBlock;
        GraphicsState graphicsState;
        static constexpr int StartTransformBits = 1 << 0;
        static constexpr int EndTransformBits = 1 << 1;
        static constexpr int AllTransformsBits = (1 << MaxTransforms) - 1;
         std::map<std::string, TransformSet> namedCoordinateSystems;
        class Transform renderFromWorld;
//        InternCache<class Transform> transformCache;
        std::vector<GraphicsState> pushedGraphicsStates;
        std::vector<std::pair<char, FileLoc>> pushStack;  // 'a': attribute, 'o': object
        struct ActiveInstanceDefinition {
            ActiveInstanceDefinition(std::string name, FileLoc loc) : entity(name, loc) {}

            std::mutex mutex;
            std::atomic<int> activeImports{ 1 };
            InstanceDefinitionSceneEntity entity;
            ActiveInstanceDefinition* parent = nullptr;
        };
        ActiveInstanceDefinition* activeInstanceDefinition = nullptr;

        // Buffer these both to avoid mutex contention and so that they are
        // consistently ordered across runs.
        std::vector<ShapeSceneEntity> shapes;
        std::vector<InstanceSceneEntity> instanceUses;

        std::set<std::string> namedMaterialNames, mediumNames;
        std::set<std::string> floatTextureNames, spectrumTextureNames, instanceNames;
        int currentMaterialIndex = 0, currentLightIndex = -1;
        SceneEntity sampler;
        SceneEntity film, integrator, filter, accelerator;
        CameraSceneEntity camera;
    };

    // this is a replacement for going into a intergrator and then wavefront aggreagate.
    // Im just putting the final parsed geom and assocaited data like textures in this one
    // place for the whole scene. I will follow the flow from RenderCPU
    class GeomData
    {
    public:
        GeomData();
        ~GeomData();

        void PrepareResourceGather(BasicScene &scene); 
        // this will call all the fns like CreateTexture, CreateGeometry (mirror to CreateAggregate)


        void ResourceGather(BasicScene& scene); 
        // this will fill GeomDatas members with all
        // the textures and geomdata 

        // going to be using BasicScenes CreateAggregate and others to set this all up

    private:
        std::map<int, TriQuadMesh> plyMeshes;
    };


}  // namespace pbrt

//#endif  // PBRT_SCENE_H
