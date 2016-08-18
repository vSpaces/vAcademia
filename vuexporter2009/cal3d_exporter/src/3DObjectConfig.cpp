#include "stdafx.h"
#define NATURA3D_API //__declspec(dllimport)
#include "Exporter.h"
#include "3DobjectConfig.h"

extern SObjectCfg objectCfg;

void CExporter::clearObjectCfg()
{
	objectCfg.boxes.clear();
	objectCfg.collision_src = "";
	objectCfg.collision_type = "";
	objectCfg.materials.clear();
	objectCfg.mesh_srcs.clear();
	objectCfg.properties.clear();
	objectCfg.scale = 0.0f;
	objectCfg.shader = "";
	objectCfg.skeleton_src = "";
}

bool CExporter::parseXML(CExportXMLFile& XMLFile, const Cal3DComString& fn)
{
	gd_xml_stream::CXMLArchive<CExportXMLFile> ar((CExportXMLFile*)&XMLFile);
	ar.SkipHeader();
	while(ar.ReadNext())
	{
		Cal3DComString sName = (const TCHAR*)ar.GetName();
		sName.MakeLower();
		if (sName.Compare("object") == 0)
		{
			Cal3DComString sAttrValue, sAttrName;
			sAttrName = ar.GetAttributeName(0);
			if (sAttrName.CompareNoCase("scale") == 0)
			{
				sAttrValue = ar.GetAttribute(sAttrName);
				float scale = atof((LPCSTR)sAttrValue);
				if( scale <= 0.0f)	scale = 1.0f;
				objectCfg.scale = scale;
			}
			else objectCfg.scale = 1.0;
		}
		else if (sName.Compare("skeleton") == 0)
		{
			Cal3DComString sAttrValue, sAttrName;
			sAttrName = ar.GetAttributeName(0);
			if (sAttrName.CompareNoCase("src") == 0)
			{
				sAttrValue = ar.GetAttribute(sAttrName);
				objectCfg.skeleton_src = sAttrValue;
			}
			else return false;	// can't find "src" attribute
		}
		else if (sName.Compare("mesh") == 0)
		{
			Cal3DComString sAttrValue, sAttrName;
			sAttrName = ar.GetAttributeName(0);
			if (sAttrName.CompareNoCase("src") == 0)
			{
				sAttrValue = ar.GetAttribute(sAttrName);
				objectCfg.mesh_srcs.push_back( sAttrValue);
			}
			else return false;	// can't find "src" attribute
		}
		else if (sName.Compare("materials") == 0)
		{
			Cal3DComString sAttrValue, sAttrName;
			sAttrName = ar.GetAttributeName(0);
			if (sAttrName.CompareNoCase("shader") == 0)
			{
				sAttrValue = ar.GetAttribute(sAttrName);
				objectCfg.shader = sAttrValue;
			}
			if (ar.HasChildren())
			{
				while (!ar.IsEndTag())
				{
					parse_meterial(ar);
				}
			}
		}
		else if (sName.Compare("properties") == 0)
		{
			if (ar.HasChildren())
			{
				while (!ar.IsEndTag())
				{
					parse_property(ar);
				}
			}
		}
		else if (sName.Compare("collisions") == 0)
		{
			Cal3DComString sAttrValue, sAttrName;
			unsigned uCount = ar.GetAttributeCount();
			for (int u=0; u<uCount; u++)
			{
				sAttrName = ar.GetAttributeName(u);
				if (sAttrName.CompareNoCase("type") == 0)
				{
					sAttrValue = ar.GetAttribute(sAttrName);
					objectCfg.collision_type = sAttrValue;
				}
				else if(sAttrName.CompareNoCase("src") == 0)
				{
					sAttrValue = ar.GetAttribute(sAttrName);
					objectCfg.collision_src = sAttrValue;
				}
			}
			if (ar.HasChildren())
			{
				while (!ar.IsEndTag())
				{
					parse_collBox(ar);
				}
			}

		}
		else if (sName.Compare("reference") == 0)
		{
			parse_reference(ar);
		}
	}	
	return true;
}

bool CExporter::parse_reference(gd_xml_stream::CXMLArchive<CExportXMLFile>& ar)
{
	while(ar.ReadNext())
	{
		Cal3DComString sName = (const TCHAR*)ar.GetName();
		if( ar.IsEndTag())
		{
			if( sName == "reference")
			{
				//??
			}
		}
	}
	return false;
}

bool CExporter::parse_meterial(gd_xml_stream::CXMLArchive<CExportXMLFile>& ar)
{
	ar.ReadNext();
	Cal3DComString sName = (const TCHAR*)ar.GetName();
	//if (ar.IsEndTag() && sName.CompareNoCase("materials")==0)
	if (ar.IsEndTag())
		return true;
	if (sName.CompareNoCase("material") == 0)
	{
		Cal3DComString sAttrValue, sAttrName;
		unsigned uCount = ar.GetAttributeCount();
		_material mtrl;
		for (int u=0; u<uCount; u++)
		{
			sAttrName = ar.GetAttributeName(u);
			sAttrName.MakeLower();
			if (sAttrName.Compare("src") == 0)
			{
				sAttrValue = ar.GetAttribute(sAttrName);
				mtrl.src = sAttrValue;
				
			}
			else if (sAttrName.Compare("shader") == 0)
			{
				sAttrValue = ar.GetAttribute(sAttrName);
				mtrl.shader = sAttrValue;
			}
		}
		if (mtrl.src.Compare("") !=0)
			objectCfg.materials.push_back(mtrl);
	}
	return true;
}

bool CExporter::parse_property(gd_xml_stream::CXMLArchive<CExportXMLFile>& ar)
{
	ar.ReadNext();
	Cal3DComString sName = (const TCHAR*)ar.GetName();
	if (ar.IsEndTag())
		return true;
	if (sName.CompareNoCase("item") == 0)
	{
		Cal3DComString sAttrValue, sAttrName;
		unsigned uCount = ar.GetAttributeCount();
		_property prop;
		for (int u=0; u<uCount; u++)
		{
			sAttrName = ar.GetAttributeName(u);
			sAttrName.MakeLower();
			if (sAttrName.Compare("name") == 0)
			{
				sAttrValue = ar.GetAttribute(sAttrName);
				prop.name = sAttrValue;
				
			}
			else if (sAttrName.Compare("value") == 0)
			{
				sAttrValue = ar.GetAttribute(sAttrName);
				prop.value = sAttrValue;
			}
			else if (sAttrName.Compare("surfaceID") == 0)
			{
				sAttrValue = ar.GetAttribute(sAttrName);
				prop.surfaceID = atoi((LPCSTR) sAttrValue);
			}
		}
		if (prop.name.Compare("") !=0 && prop.value.Compare("") !=0)
			objectCfg.properties.push_back(prop);
	}
	return true;
}

bool CExporter::parse_collBox(gd_xml_stream::CXMLArchive<CExportXMLFile>& ar)
{
	ar.ReadNext();
	Cal3DComString sName = (const TCHAR*)ar.GetName();
	if (ar.IsEndTag())
		return true;
	if (sName.CompareNoCase("box") == 0)
	{
		Cal3DComString sAttrValue, sAttrName;
		unsigned uCount = ar.GetAttributeCount();
		box3d box;
		for (int u=0; u<uCount; u++)
		{
			sAttrName = ar.GetAttributeName(u);
			sAttrName.MakeLower();
			if (sAttrName.Compare("min") == 0)
			{
				sAttrValue = ar.GetAttribute(sAttrName);
				box.min.x = atof((LPCSTR)sAttrValue.Left(sAttrValue.Find(";")));
				box.min.y = atof((LPCSTR)sAttrValue.Right(sAttrValue.GetLength()-sAttrValue.Find(";")-1).Left(sAttrValue.Find(";")));
				box.min.z = atof((LPCSTR)sAttrValue.Right(sAttrValue.GetLength()-sAttrValue.ReverseFind(';')-1));
			}
			else if (sAttrName.Compare("max") == 0)
			{
				sAttrValue = ar.GetAttribute(sAttrName);
				box.max.x = atof((LPCSTR)sAttrValue.Left(sAttrValue.Find(";")));
				box.max.y = atof((LPCSTR)sAttrValue.Right(sAttrValue.GetLength()-sAttrValue.Find(";")-1).Left(sAttrValue.Find(";")));
				box.max.z = atof((LPCSTR)sAttrValue.Right(sAttrValue.GetLength()-sAttrValue.ReverseFind(';')-1));
			}
		}
		objectCfg.boxes.push_back(box);
	}
	return true;
}

bool CExporter::saveStringToFile(CFile* file, Cal3DComString str)
{
	assert( file);
	file->Write(str.GetBuffer(), str.GetLength());	
	return true;
}

bool CExporter::saveRefXMLConfig(const Cal3DComString& fn)
{
	assert( objectCfg.is_reference);
	CFile xmlConfig;
	try
	{
		if( !xmlConfig.Open(fn.GetString(), CFile::modeCreate | CFile::modeWrite, NULL))
		{
			return false;
		}
		Cal3DComString	str;
		saveStringToFile( &xmlConfig, Cal3DComString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
		str.Format("<reference ref_name=\"%s\">\n", objectCfg.ref_name.GetBuffer());
		saveStringToFile( &xmlConfig, str);
			str.Format("<translation x=\"%.2f\" y=\"%.2f\" z=\"%.2f\"/>\n", objectCfg.ref_translation.x, objectCfg.ref_translation.y, objectCfg.ref_translation.z);
			saveStringToFile( &xmlConfig, str);
			str.Format("<rotation x=\"%.2f\" y=\"%.2f\" z=\"%.2f\" w=\"%.2f\"/>\n", objectCfg.ref_rotation.x, objectCfg.ref_rotation.y, objectCfg.ref_rotation.z, objectCfg.ref_rotation.w);
			saveStringToFile( &xmlConfig, str);
			str.Format("<scale x=\"%.2f\" y=\"%.2f\" z=\"%.2f\"/>\n", objectCfg.ref_scale.x, objectCfg.ref_scale.y, objectCfg.ref_scale.z);
			saveStringToFile( &xmlConfig, str);
		saveStringToFile( &xmlConfig, Cal3DComString("</reference>\n"));
	}
	catch(CFileExException e)
	{
		return false;
	}
	return true;
}

bool CExporter::saveXMLConfig(Cal3DComString fn)
{
	CFile xmlConfig;
	try
	{
		xmlConfig.Open(fn.GetString(), CFile::modeCreate | CFile::modeWrite, NULL);

		Cal3DComString xmlHeader = Cal3DComString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

		Cal3DComString xmlNodeObjectStartTag;
		xmlNodeObjectStartTag.Format("<object scale=\"%f\">\n", objectCfg.scale);
		Cal3DComString xmlNodeObjectEndTag = Cal3DComString("</object>\n");

		Cal3DComString xmlNodeSkeleton;
		xmlNodeSkeleton.Format("	<skeleton src=\"%s\"/>\n", objectCfg.skeleton_src.GetString());


		Cal3DComString xmlNodeMaterialsStartTag = Cal3DComString("	<materials");
		if (objectCfg.shader.CompareNoCase("") != 0)
		{
			xmlNodeMaterialsStartTag += " shader=\"" + objectCfg.shader + "\"";
		}
		xmlNodeMaterialsStartTag += ">\n";
		Cal3DComString xmlNodeMaterialsEndTag = Cal3DComString("	</materials>\n");
		
		Cal3DComString xmlMaterials;
		std::vector<_material>::iterator vci_m = objectCfg.materials.begin();
		for ( ; vci_m != objectCfg.materials.end(); vci_m++)
		{
			Cal3DComString xmlNodeMaterial;
			xmlNodeMaterial.Format("		<material src=\"%s\"", vci_m->src.GetBuffer());
			if (vci_m->shader.CompareNoCase("") != 0)
			{
				xmlNodeMaterial += " shader=\"" + vci_m->shader + "\"";
			}
			xmlNodeMaterial += "/>\n";
			xmlMaterials += xmlNodeMaterial;
		}

		Cal3DComString xmlNodePropertiesStartTag = Cal3DComString("	<properties>\n");
		Cal3DComString xmlNodePropertiesEndTag = Cal3DComString("	</properties>\n");

		Cal3DComString xmlProperties;
		if (objectCfg.properties.size() != 0)
		{
			std::vector<_property>::iterator vci_p = objectCfg.properties.begin();
			for ( ; vci_p != objectCfg.properties.end(); vci_p++)
			{
				Cal3DComString xmlNodeProperty;
				xmlNodeProperty.Format("		<item name=\"%s\" value=\"%s\"", vci_p->name.GetString(), vci_p->value.GetString());
				if (vci_p->surfaceID != -1)
				{
					xmlNodeProperty += " surfaceID=\"" + Cal3DComString((LPTSTR)vci_p->surfaceID) + "\"";
				}
				xmlNodeProperty += "/>\n";
				xmlProperties += xmlNodeProperty;
			}
		}

		Cal3DComString xmlNodeCollisionStartTag = Cal3DComString("	<collisions");
		if (objectCfg.collision_type.Compare("") != 0)
		{
			xmlNodeCollisionStartTag += " type=\"" + objectCfg.collision_type + "\"";
		}
		if (objectCfg.collision_src.Compare("") != 0)
		{
			xmlNodeCollisionStartTag += " src=\"" + objectCfg.collision_src + "\"";
		}
		xmlNodeCollisionStartTag += ">\n";
		Cal3DComString xmlNodeCollisionEndTag = Cal3DComString("	</collisions>\n");

		Cal3DComString xmlBoxes;
		if (objectCfg.boxes.size() != 0)
		{
			std::vector<box3d>::const_iterator vci_b = objectCfg.boxes.begin();
			for ( ; vci_b != objectCfg.boxes.end(); vci_b++)
			{
				Cal3DComString xmlNodeBox;
				Cal3DComString min;
				Cal3DComString max;
				min.Format("%f;%f;%f", vci_b->min.x, vci_b->min.y, vci_b->min.z);
				max.Format("%f;%f;%f", vci_b->max.x, vci_b->max.y, vci_b->max.z);
				xmlNodeBox.Format("		<box min=\"%s\" max=\"%s\"", min.GetString(), max.GetString());
				xmlNodeBox += "/>\n";
				xmlBoxes += xmlNodeBox;
			}
		}

		int size = xmlHeader.GetLength();
		std::string str = xmlHeader.GetString();
		xmlConfig.Write(xmlHeader.GetString(), xmlHeader.GetLength());

		xmlConfig.Write(xmlNodeObjectStartTag.GetBuffer(), xmlNodeObjectStartTag.GetLength());

		if (objectCfg.skeleton_src.Compare("") != 0)
			xmlConfig.Write(xmlNodeSkeleton.GetBuffer(), xmlNodeSkeleton.GetLength());

		//удалить дублирующие меши
		std::vector<Cal3DComString> mesh_srcs = objectCfg.mesh_srcs;
		for (int i = 0; i < mesh_srcs.size(); i++)
		{
			Cal3DComString meshName = mesh_srcs[i];
			for (int j = i+1; j < mesh_srcs.size(); j++)
			{
				if (meshName.Compare(mesh_srcs[j]) == 0)
				{
					mesh_srcs.erase(mesh_srcs.begin() + j);
					j --;
				}
			}
		}

		for (int imesh=0; imesh<mesh_srcs.size(); imesh++)
		{
			Cal3DComString xmlNodeMesh;
			xmlNodeMesh.Format("	<mesh src=\"%s\"/>\n", mesh_srcs[imesh].GetString());
			xmlConfig.Write(xmlNodeMesh.GetBuffer(), xmlNodeMesh.GetLength());
		}

		/*for (int imesh=0; imesh<objectCfg.mesh_srcs.size(); imesh++)
		{
			Cal3DComString xmlNodeMesh;
			xmlNodeMesh.Format("	<mesh src=\"%s\"/>\n", objectCfg.mesh_srcs[imesh].GetString());
			xmlConfig.Write(xmlNodeMesh.GetBuffer(), xmlNodeMesh.GetLength());
		}*/

		if (objectCfg.materials.size() != 0)
		{
			xmlConfig.Write(xmlNodeMaterialsStartTag.GetString(), xmlNodeMaterialsStartTag.GetLength());
			xmlConfig.Write(xmlMaterials.GetString(), xmlMaterials.GetLength());
			xmlConfig.Write(xmlNodeMaterialsEndTag.GetString(), xmlNodeMaterialsEndTag.GetLength());
		}

		if (objectCfg.properties.size() != 0)
		{
			xmlConfig.Write(xmlNodePropertiesStartTag.GetString(), xmlNodePropertiesStartTag.GetLength());
			xmlConfig.Write(xmlProperties.GetString(), xmlProperties.GetLength());
			xmlConfig.Write(xmlNodePropertiesEndTag.GetString(), xmlNodePropertiesEndTag.GetLength());
		}

		if (objectCfg.collision_type.Compare("") !=0 ||objectCfg.collision_src.Compare("") !=0)
		{
			xmlConfig.Write(xmlNodeCollisionStartTag.GetString(), xmlNodeCollisionStartTag.GetLength());
			if (objectCfg.boxes.size() != 0)
			{
				xmlConfig.Write(xmlBoxes.GetString(), xmlBoxes.GetLength());
			}
			xmlConfig.Write(xmlNodeCollisionEndTag.GetString(), xmlNodeCollisionEndTag.GetLength());
		}

		xmlConfig.Write(xmlNodeObjectEndTag.GetString(), xmlNodeObjectEndTag.GetLength());
		xmlConfig.Close();
	} catch(CFileExException e){}
	return true;
}