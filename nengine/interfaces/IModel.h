
#pragma once

#include <string>

class IModel
{
public:
	// Simply set texture for part of model, starting with polygon with number 'offset'
	virtual void AddTexture(int offset, std::string name) = 0;
	// Set count of vertices, only after that memory for vertices, normals, texture coords is allocated	
	virtual void SetVertexCount(int count) = 0;
	// Set count of faces, after that memory for indices is allocated
	virtual void SetFaceCount(int count) = 0;
	// Get count of vertices
	virtual int GetVertexCount()const = 0;
	// Get count of faces
	virtual int GetFaceCount()const = 0;
	// Calculate bounding box immediately
	virtual void ReCountBoundingBox() = 0;
	// Add material for part of model
	virtual void AddMaterial(std::string materialName) = 0;
	virtual void AddMaterialSafety(std::string materialName) = 0;
	// Return pointer to array of vertices
	virtual float* GetVertexPointer() = 0;
	// Return pointer to array of normals
	virtual float* GetNormalPointer() = 0;
	// Return pointer to array of texture coordinates
	virtual float* GetTextureCoordsPointer(int level) = 0;
	// Return pointer to array of indices
	virtual unsigned short* GetIndexPointer() = 0;
	// Get name of the model
	virtual char* GetModelName() = 0;
	// Set face orient
	virtual void SetFaceOrient(int orient) = 0;
	// Set two sided property
	virtual void SetTwoSided(int materialID, bool isTwoSided) = 0;    
	//
	virtual bool Lock() = 0;
	//
	virtual void Unlock() = 0;
};