#include "PBR_include/scene.h"
#include "PBR_include/file.h"

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

    void BasicSceneBuilder::Transform(float tr[16], FileLoc loc) {
        graphicsState.ForActiveTransforms([=](auto t) {
            return Transpose(pbrt::Transform(SquareMatrix<4>(pstd::MakeSpan(tr, 16))));
            });
    } // this needs work



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

            const class Transform* renderFromObject = RenderFromObeject(0); // this needs to have the 
            // world space transformation from cta, see scene.h 476 in original branch
            const class Transform* objectFromRender{}; 

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


	
}