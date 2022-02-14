#pragma once

#include <filesystem>

#include <cuiui/math/types.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct AssimpModelSource {
    struct Vertex {
        f32vec3 pos, nrm;
        f32vec2 tex;
    };
    using Index = uint32_t;
    struct Node {
        std::vector<Index> indices;
        std::vector<Vertex> vertices;
        f32mat4 modl_mat;
        // f32mat4 norm_mat;
    };

    std::filesystem::path path;
    std::vector<Node> nodes;
    std::vector<Vertex> vertices;
    std::vector<std::filesystem::path> albedo_texture_paths, normal_texture_paths;

    AssimpModelSource(const std::filesystem::path &path) : path(path) {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path.string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return;
        }
        process_node(scene->mRootNode, scene, path.parent_path());
    }

    void process_node(aiNode *node, const aiScene *scene, const std::filesystem::path &root_dir);
};

static std::vector<std::shared_ptr<AssimpModelSource>> model_sources;

struct AssimpModel {
    std::shared_ptr<AssimpModelSource> source;

    AssimpModel(const std::filesystem::path &path) {
        auto find_iter = std::find_if(model_sources.begin(), model_sources.end(), [&](const std::shared_ptr<AssimpModelSource> &src) -> bool {
            return src->path == path;
        });

        if (find_iter == model_sources.end()) {
            source = model_sources.emplace_back(std::make_shared<AssimpModelSource>(path));
        } else {
            source = *find_iter;
        }
    }
};

void AssimpModelSource::process_node(aiNode *node, const aiScene *scene, const std::filesystem::path &root_dir) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        nodes.push_back({});
        auto &mesh_node = nodes.back();
        mesh_node.modl_mat = *reinterpret_cast<f32mat4 *>(&node->mTransformation);
        // mesh_node.norm_mat = inverse(mesh_node.modl_mat);
        mesh_node.modl_mat = transpose(mesh_node.modl_mat);
        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            Vertex vertex;
            vertex.pos = *reinterpret_cast<f32vec3 *>(mesh->mVertices + j);
            vertex.nrm = *reinterpret_cast<f32vec3 *>(mesh->mNormals + j);
            vertex.tex = *reinterpret_cast<f32vec2 *>(mesh->mTextureCoords[0] + j);
            mesh_node.vertices.push_back(vertex);
        }
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++)
                mesh_node.indices.push_back(face.mIndices[k]);
        }
        auto get_texture_paths = [&root_dir](auto material, auto type) {
            std::vector<std::filesystem::path> paths;
            auto n = material->GetTextureCount(type);
            paths.reserve(n);
            for (size_t i = 0; i < n; ++i) {
                aiString relpath;
                material->GetTexture(type, static_cast<u32>(i), &relpath);
                paths.push_back(root_dir / std::filesystem::path(relpath.C_Str()));
            }
            return paths;
        };
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        albedo_texture_paths = get_texture_paths(material, aiTextureType_DIFFUSE);
        normal_texture_paths = get_texture_paths(material, aiTextureType_NORMALS);
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
        process_node(node->mChildren[i], scene, root_dir);
}
