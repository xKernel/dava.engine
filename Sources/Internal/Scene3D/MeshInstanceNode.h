/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#ifndef __DAVAENGINE_MESH_INSTANCE_H__
#define __DAVAENGINE_MESH_INSTANCE_H__

#include "Scene3D/SceneNode.h"

namespace DAVA 
{
class Scene;
class StaticMesh;
class Material;
class Texture;
class SceneFileV2;
    
class MeshInstanceNode : public SceneNode
{
public:	
	MeshInstanceNode(Scene * _scene = 0);
	~MeshInstanceNode();
	
	void AddPolygonGroup(StaticMesh * mesh, int32 polygonGroupIndex, Material* material);
    void AddPolygonGroupForLayer(int32 layer, StaticMesh * mesh, int32 polygonGroupIndex, Material* material);
    void AddDummyLODLayer(int32 layer);

    virtual void Update(float32 timeElapsed);
	virtual void Draw();
	
	inline void SetVisible(bool isVisible);
	inline bool GetVisible();
	
	inline AABBox3 & GetBoundingBox();
	
	Vector<StaticMesh*> & GetMeshes()
	{
		return lodLayers.begin()->meshes;
	}

	Vector<int32> & GetPolygonGroupIndexes()
	{
		return lodLayers.begin()->polygonGroupIndexes;
	}

    Vector<Material*> & GetMaterials()
	{
		return lodLayers.begin()->materials;
	}
        
//	Vector<StaticMesh*> & GetMeshes(int32 lodLayer)
//	{
//		return lodLayers.begin()->meshes;
//	}
	
    virtual SceneNode* Clone(SceneNode *dstNode = NULL);
//    virtual SceneNode* Clone();
    
    //Returns maximum Bounding Box as WorlTransformedBox
    virtual AABBox3 GetWTMaximumBoundingBox();

	
    /**
        \brief virtual function to save node to KeyedArchive
     */
    virtual void Save(KeyedArchive * archive, SceneFileV2 * sceneFile);
    
    /**
        \brief virtual function to load node to KeyedArchive
     */
	virtual void Load(KeyedArchive * archive, SceneFileV2 * sceneFile);

	/**
        \brief Add lightmap texture.
        Consequent calls of this function will add lightmaps to consequent PolygonGroups of this MeshInstance.
            
        Normal usage (pseudocode):
        \code
        ClearLightmaps();
        for(each polygon group)
        {
            AddLightmap(...);
        }
        \endcode

        \param[in] lightmapName path to texture
	 */
	void AddLightmap(const String & lightmapName);

	/**
        \brief Delete all lightmaps for this MeshInstance. 
	 */
	void ClearLightmaps();
    
	/**
        \brief Replace material for polygon group. 
	 */
    void ReplaceMaterial(Material *material, int32 index);

	void CreateDynamicShadowNode();
	void DeleteDynamicShadowNode();
    
    virtual void GetDataNodes(Set<DataNode*> & dataNodes);


    Texture * GetLightmapForIndex(int32 index);
    int32 GetLightmapCount();

    virtual void BakeTransforms();

protected:
//    virtual SceneNode* CopyDataTo(SceneNode *dstNode);

    struct LodData
    {
        Vector<StaticMesh*> meshes;
        Vector<int32> polygonGroupIndexes;
        Vector<Material*> materials;
        int layer;
    };
    
    LodData *currentLod;
    List<LodData> lodLayers;
    
    
	AABBox3 bbox;
    AABBox3 transformedBox;
    
    bool lodPresents;
    int lastLodUpdateFrame;

	struct LightmapData
	{
		Texture * lightmap;
		String lightmapName;
	};
	Vector<LightmapData> lightmaps;

};
	
inline AABBox3 & MeshInstanceNode::GetBoundingBox()
{
	return bbox;
}

};

#endif // __DAVAENGINE_MESH_INSTANCE_H__
