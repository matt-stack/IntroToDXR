#pragma once
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <utility>

#include "PBR_include/parser.h"
#include "PBR_include/math.h"
namespace PBR {
    struct TriQuadMesh {
        // TriQuadMesh Public Methods
        static TriQuadMesh ReadPLY(const std::string& filename);

        void ConvertToOnlyTriangles();
        void ComputeNormals(); // new normals post displacement

        std::string ToString() const;

        template <typename Dist, typename Disp>
        TriQuadMesh Displace(Dist&& dist, float maxDist, Disp&& displace,
            const FileLoc* loc = nullptr) const {
            if (uv.empty())
                ErrorExit(loc, "Vertex uvs are currently required by Displace(). Sorry.\n");

            // Prepare the output mesh
            TriQuadMesh outputMesh = *this;
            outputMesh.ConvertToOnlyTriangles();
            if (outputMesh.n.empty())
                outputMesh.ComputeNormals();
            outputMesh.triIndices.clear();

            // Refine
            //HashMap<std::pair<int, int>, int, HashIntPair> edgeSplit({});
            std::unordered_map<std::pair<int, int>, int> edgeSplit({});
            for (int i = 0; i < triIndices.size() / 3; ++i)
                outputMesh.Refine(dist, maxDist, triIndices[3 * i], triIndices[3 * i + 1],
                    triIndices[3 * i + 2], edgeSplit);

            // Displace
            displace(outputMesh.p.data(), outputMesh.n.data(), outputMesh.uv.data(),
                outputMesh.p.size());

            outputMesh.ComputeNormals();

            return outputMesh;
        }

        std::vector<Point3f> p;
        std::vector<Normal3f> n;
        std::vector<Point2f> uv;
        std::vector<int> faceIndices;
        std::vector<int> triIndices, quadIndices;

    private:
        // TriQuadMesh Private Methods
        template <typename Dist>
        void Refine(Dist&& distance, float maxDist, int v0, int v1, int v2,
            std::unordered_map<std::pair<int, int>, int>& edgeSplit) {
            //HashMap<std::pair<int, int>, int, HashIntPair>& edgeSplit) {
            Point3f p0 = p[v0], p1 = p[v1], p2 = p[v2];
            Float d01 = distance(p0, p1), d12 = distance(p1, p2), d20 = distance(p2, p0);

            if (d01 < maxDist && d12 < maxDist && d20 < maxDist) {
                triIndices.push_back(v0);
                triIndices.push_back(v1);
                triIndices.push_back(v2);
                return;
            }

            // order so that the first two vertices have the longest edge
            std::array<int, 3> v;
            if (d01 > d12) {
                if (d01 > d20)
                    v = { v0, v1, v2 };
                else
                    v = { v2, v0, v1 };
            }
            else {
                if (d12 > d20)
                    v = { v1, v2, v0 };
                else
                    v = { v2, v0, v1 };
            }

            // has the edge been spilt before?
            std::pair<int, int> edge(v[0], v[1]);
            if (v[0] > v[1])
                std::swap(edge.first, edge.second);

            int vmid;
            if (edgeSplit.HasKey(edge)) {
                vmid = edgeSplit[edge];
            }
            else {
                vmid = p.size();
                edgeSplit.Insert(edge, vmid);
                p.push_back((p[v[0]] + p[v[1]]) / 2);
                if (!n.empty()) {
                    Normal3f nn = n[v[0]] + n[v[1]];
                    if (LengthSquared(nn) > 0)
                        nn = Normalize(nn);
                    n.push_back(nn);
                }
                if (!uv.empty())
                    uv.push_back((uv[v[0]] + uv[v[1]]) / 2);
            }

            Refine(distance, maxDist, v[0], vmid, v[2], edgeSplit);
            Refine(distance, maxDist, vmid, v[1], v[2], edgeSplit);
        }
    };
} // namespace

