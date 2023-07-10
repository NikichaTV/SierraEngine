//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Mesh.h"

namespace Sierra::Engine
{

    /* --- CONSTRUCTORS --- */

    Mesh::Mesh(const MeshCreateInfo &createInfo)
        : vertexCount(createInfo.vertices.size()), indexCount(createInfo.indices.size())
    {
        CreateVertexBuffer(createInfo.vertices);
        CreateIndexBuffer(createInfo.indices);

        totalMeshCount++;
        totalVertexCount += vertexCount;
    }

    SharedPtr<Mesh> Mesh::Create(MeshCreateInfo createInfo)
    {
        return std::make_shared<Mesh>(createInfo);
    }

    /* --- SETTER METHODS --- */

    void Mesh::CreateVertexBuffer(std::vector<Vertex> &givenVertices)
    {
        // Calculate the buffer size
        uint64 bufferSize = sizeof(Vertex) * givenVertices.size();

        auto stagingBuffer = Rendering::Buffer::Create({
            .memorySize = bufferSize,
            .bufferUsage = Rendering::BufferUsage::TRANSFER_SRC
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenVertices.data());

        vertexBuffer = Rendering::Buffer::Create({
           .memorySize = bufferSize,
           .bufferUsage = Rendering::BufferUsage::TRANSFER_DST | Rendering::BufferUsage::VERTEX
       });

        stagingBuffer->CopyToBuffer(vertexBuffer);

        stagingBuffer->Destroy();
    }

    void Mesh::CreateIndexBuffer(std::vector<INDEX_BUFFER_TYPE> &givenIndices)
    {
        // Calculate the buffer size
        uint64 bufferSize = UINT_SIZE * givenIndices.size();

        auto stagingBuffer = Rendering::Buffer::Create({
            .memorySize = bufferSize,
            .bufferUsage = Rendering::BufferUsage::TRANSFER_SRC
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenIndices.data());

        indexBuffer = Rendering::Buffer::Create({
             .memorySize = bufferSize,
             .bufferUsage = Rendering::BufferUsage::TRANSFER_DST | Rendering::BufferUsage::INDEX
         });

        stagingBuffer->CopyToBuffer(indexBuffer);

        stagingBuffer->Destroy();
    }

    /* --- DESTRUCTOR --- */

    void Mesh::Destroy()
    {
        vertexBuffer->Destroy();
        indexBuffer->Destroy();

        totalMeshCount--;
        totalVertexCount -= vertexCount;
    }
}