#include "PBR_include/scene.h"
#include "PBR_include/file.h"
#include "PBR_include/primative.h"
#include <iterator>
#include <algorithm>

namespace PBR{


	// BasicScene

    BasicScene::BasicScene() {
    //not empty
    };
/*
    void BasicScene::startLoadingNormalMaps(const ParameterDictionary& parameters) {
        std::string filename = ResolveFilename(parameters.GetOneString("normalmap", ""));
        if (filename.empty())
            return;

//        // Overload materialMutex, which we already hold, for the futures...
//        if (normalMapJobs.find(filename) != normalMapJobs.end())
//            // It's already in flight.
//            return;
            
        auto create = [=](std::string filename) {
            Allocator alloc = threadAllocators.Get();
            ImageAndMetadata immeta =
                Image::Read(filename, Allocator(), ColorEncoding::Linear);
            Image& image = immeta.image;
            ImageChannelDesc rgbDesc = image.GetChannelDesc({ "R", "G", "B" });
            if (!rgbDesc)
                ErrorExit("%s: normal map image must contain R, G, and B channels", filename);
            Image* normalMap = alloc.new_object<Image>(alloc);
            *normalMap = image.SelectChannels(rgbDesc);

            return normalMap; // return is type Image
        };
        normalMapJobs[filename] = RunAsync(create, filename);
    }
*/

    int BasicScene::AddMaterial(SceneEntity material) {
        //std::lock_guard<std::mutex> lock(materialMutex);
//        startLoadingNormalMaps(material.parameters); // mstack TODO When we want normals tackle this, add image.cpp and image.h
        materials.push_back(std::move(material));
        return int(materials.size() - 1);
    }

    //void BasicScene::AddShapes(PBR::span<ShapeSceneEntity> s)  {
    void BasicScene::AddShapes(std::vector<PBR::ShapeSceneEntity> s)  {
       // std::lock_guard<std::mutex> lock(shapeMutex);
        //std::move(std::begin(s), std::end(s), std::back_inserter(this->shapes)); // what is this triple argument std::move thing?
        std::move(s.begin(), s.end(), std::back_inserter(this->shapes)); // what is this triple argument std::move thing?
        // Update, this is actually std::move algorithm, not std::move the utility
        // which moves from beginning to end into the thrid, which in this case is a 
        // back_inserter, pretty cool! Though now that I changed this to just take a vector
        // rather than a PBR::span, it could use std vector's insert to concat this vector,
        // but I like using std::move now
    }


    // I dont think I need this for Instance Acceleration Structures 
//    void BasicScene::AddInstanceUses(PBR::span<InstanceSceneEntity> in) {
//        std::lock_guard<std::mutex> lock(instanceUseMutex);
//        std::move(std::begin(in), std::end(in), std::back_inserter(instances));
//    }


	// BasicSceneBuilder

    BasicSceneBuilder::GraphicsState::GraphicsState() {
        currentMaterialIndex = 0;
    }


    BasicSceneBuilder::BasicSceneBuilder(BasicScene* scene)
        : scene(scene)
    {
        // Set scene defaults
//        camera.name = SceneEntity::internedStrings.Lookup("perspective");
//        sampler.name = SceneEntity::internedStrings.Lookup("zsobol");
//        filter.name = SceneEntity::internedStrings.Lookup("gaussian");
//        integrator.name = SceneEntity::internedStrings.Lookup("volpath");
//        accelerator.name = SceneEntity::internedStrings.Lookup("bvh");

//        film.name = SceneEntity::internedStrings.Lookup("rgb");
//        film.parameters = ParameterDictionary({}, RGBColorSpace::sRGB);

        ParameterDictionary dict({}, {});
        currentMaterialIndex = scene->AddMaterial(SceneEntity("diffuse", dict, {}));
    }

    void BasicSceneBuilder::AttributeBegin(FileLoc loc) {
        //VERIFY_WORLD("AttributeBegin");
        pushedGraphicsStates.push_back(graphicsState);
        pushStack.push_back(std::make_pair('a', loc));
    }

    void BasicSceneBuilder::AttributeEnd(FileLoc loc) {
        //VERIFY_WORLD("AttributeEnd");
        // Issue error on unmatched _AttributeEnd_
        if (pushedGraphicsStates.empty()) {
            //Error(&loc, "Unmatched AttributeEnd encountered. Ignoring it.");
            return;
        }

        // NOTE: must keep the following consistent with code in ObjectEnd
        graphicsState = std::move(pushedGraphicsStates.back());
        pushedGraphicsStates.pop_back();

        if (pushStack.back().first == 'o') {
            //ErrorExitDeferred(&loc,
                //"Mismatched nesting: open ObjectBegin from %s at AttributeEnd",
               // pushStack.back().second);
        }
        else {
            CHECK_EQ(pushStack.back().first, 'a');
        }
        pushStack.pop_back();
    }


    void BasicSceneBuilder::Texture(const std::string& origName, const std::string& type,
        const std::string& texname, ParsedParameterVector params,
        FileLoc loc) {
        //std::string name = NormalizeUTF8(origName);
        std::string name = origName; // just use the original, this might be a source of error 
        //VERIFY_WORLD("Texture");

        ParameterDictionary dict(std::move(params), graphicsState.textureAttributes);
            //graphicsState.colorSpace);

        if (type != "float" && type != "spectrum") {
//            ErrorExitDeferred(
//                &loc, "%s: texture type unknown. Must be \"float\" or \"spectrum\".", type);
            return;
        }

        std::set<std::string>& names =
            (type == "float") ? floatTextureNames : spectrumTextureNames;
        if (names.find(name) != names.end()) {
//            ErrorExitDeferred(&loc, "Redefining texture \"%s\".", name);
            return;
        }
        names.insert(name);

        if (type == "float") { // mstack These need to be implmeneted for textures, see original scene.cpp line 913
//            scene->AddFloatTexture(
//                name, TextureSceneEntity(texname, std::move(dict), loc, RenderFromObject()));
        }
        else { // mstack these need to be implmeneted for textures
//            scene->AddSpectrumTexture(
//                name, TextureSceneEntity(texname, std::move(dict), loc, RenderFromObject()));
        }
    }

    void BasicSceneBuilder::Material(const std::string& name, ParsedParameterVector params,
        FileLoc loc) {
        //VERIFY_WORLD("Material");

        ParameterDictionary dict(std::move(params), graphicsState.materialAttributes);
//            graphicsState.colorSpace);

        graphicsState.currentMaterialIndex =
            scene->AddMaterial(SceneEntity(name, std::move(dict), loc));
        graphicsState.currentMaterialName.clear();
    }

    void BasicSceneBuilder::NamedMaterial(const std::string& origName, FileLoc loc) {
        //std::string name = NormalizeUTF8(origName);
        std::string name = origName; // just use the original 
        //VERIFY_WORLD("NamedMaterial");
        graphicsState.currentMaterialName = name;
        graphicsState.currentMaterialIndex = -1;
    }

    void BasicSceneBuilder::AreaLightSource(const std::string& name,
        ParsedParameterVector params, FileLoc loc) {
        //VERIFY_WORLD("AreaLightSource");
        graphicsState.areaLightName = name;
        graphicsState.areaLightParams = ParameterDictionary(
            std::move(params), graphicsState.lightAttributes);
            //std::move(params), graphicsState.lightAttributes, graphicsState.colorSpace); // dropping the colorSpace
        graphicsState.areaLightLoc = loc;
    }
      

    // Transform() as inheritted from ParserTarget
    void BasicSceneBuilder::Transform(float tr[16], FileLoc loc) {
        graphicsState.ForActiveTransforms([=](auto t) {
            return PBR::Transpose(PBR::Transform(SquareMatrix<4>(PBR::MakeSpan(tr, 16))));
            });
    } // this should work now?

    void BasicSceneBuilder::Rotate(float angle, float dx, float dy, float dz, FileLoc loc) {
        graphicsState.ForActiveTransforms(
            [=](auto t) { return t * PBR::Rotate(angle, Vector3f(dx, dy, dz)); });
    }

    void BasicSceneBuilder::Scale(float sx, float sy, float sz, FileLoc loc) {
        graphicsState.ForActiveTransforms(
            [=](auto t) { return t * PBR::Scale(sx, sy, sz); });
    }




    void BasicSceneBuilder::Shape(const std::string& name, ParsedParameterVector params,
        FileLoc loc) {
        //VERIFY_WORLD("Shape");

        ParameterDictionary dict(std::move(params), graphicsState.shapeAttributes);
//            graphicsState.colorSpace);

        int areaLightIndex = -1;
        /*
        if (!graphicsState.areaLightName.empty()) {
            areaLightIndex = scene->AddAreaLight(SceneEntity(graphicsState.areaLightName,
                graphicsState.areaLightParams,
                graphicsState.areaLightLoc));
            if (activeInstanceDefinition)
                //Warning(&loc, "Area lights not supported with object instancing");
        }
        */
        if (0) {
            /*
        if (CTMIsAnimated()) {
            AnimatedTransform renderFromShape = RenderFromObject();
            const class Transform* identity = transformCache.Lookup(pbrt::Transform());

            AnimatedShapeSceneEntity entity(
                { name, std::move(dict), loc, renderFromShape, identity,
                 graphicsState.reverseOrientation, graphicsState.currentMaterialIndex,
                 graphicsState.currentMaterialName, areaLightIndex,
                 graphicsState.currentInsideMedium, graphicsState.currentOutsideMedium });

            if (activeInstanceDefinition)
                activeInstanceDefinition->entity.animatedShapes.push_back(std::move(entity));
            else
                scene->AddAnimatedShape(std::move(entity));
                */
        }
        else {
//            const class Transform* renderFromObject =
//                transformCache.Lookup(RenderFromObject(0));
//            const class Transform* objectFromRender =
//                transformCache.Lookup(Inverse(*renderFromObject));

            //const class Transform* renderFromObject{};
            const class Transform rtemp = RenderFromObject(0);
            const class Transform* renderFromObject = &rtemp; // this needs to have the 
            // world space transformation from cta, see scene.h 476 in original branch
            const class Transform* objectFromRender{}; // this can be default

            ShapeSceneEntity entity(
                { name, std::move(dict), loc, renderFromObject, objectFromRender,
                 graphicsState.reverseOrientation, graphicsState.currentMaterialIndex,
                 graphicsState.currentMaterialName, areaLightIndex,
                 graphicsState.currentInsideMedium, graphicsState.currentOutsideMedium });
            if (activeInstanceDefinition)
                activeInstanceDefinition->entity.shapes.push_back(std::move(entity));
            else
                shapes.push_back(std::move(entity));
        }
    }

    void BasicSceneBuilder::WorldBegin(FileLoc loc) {
        //VERIFY_OPTIONS("WorldBegin");
        // Reset graphics state for _WorldBegin_
        currentBlock = BlockState::WorldBlock;
        for (int i = 0; i < MaxTransforms; ++i)
            graphicsState.ctm[i] = PBR::Transform();
        graphicsState.activeTransformBits = AllTransformsBits;
        namedCoordinateSystems["world"] = graphicsState.ctm;

        // Pass pre-_WorldBegin_ entities to _scene_
        //scene->SetOptions(filter, film, camera, sampler, integrator, accelerator);
    }


    void BasicSceneBuilder::EndOfFiles() {
        /*
        if (currentBlock != BlockState::WorldBlock)
            ErrorExitDeferred("End of files before \"WorldBegin\".");

        // Ensure there are no pushed graphics states
        while (!pushedGraphicsStates.empty()) {
            ErrorExitDeferred("Missing end to AttributeBegin");
            pushedGraphicsStates.pop_back();
        }

        if (errorExit)
            ErrorExit("Fatal errors during scene construction");
            */

        if (!shapes.empty())
            scene->AddShapes(shapes);
//        if (!instanceUses.empty()) // We dont not follow this path of IAS 
//            scene->AddInstanceUses(instanceUses);

        //scene->Done(); //this Done is just an empty function in the original

    }

// GeomData class functions

    GeomData::GeomData()
    { // empty init for now
    }

    GeomData::~GeomData()
    {
    }

    void GeomData::PrepareResourceGather(BasicScene& scene) {
        // going to be calling BasicScene member functions

        // Textures
        NamedTextures textures = scene.CreateTextures();

        // Lights
//        std::map<int, std::vector<Light>*> shapeIndexToAreaLights;
//        std::vector<Light> lights =
//            scene.CreateLights(textures, &shapeIndexToAreaLights);

        // Materials
        std::map<std::string, PBR::Material> namedMaterials;
        std::vector<PBR::Material> materials;
        scene.CreateMaterials(textures, &namedMaterials, &materials);

        // Geometry
        std::vector<Primative> geometry;
        scene.CreateAggregate(textures, shapeIndexToAreaLights,
            namedMaterials, materials);



    }

	
}