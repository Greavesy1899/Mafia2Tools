#include "M2EDM.h"
#include "M2Helpers.h"

//===================================================
//		EDMPART
//===================================================
void EDMPart::SetName(std::wstring name) {
	EDMPart::name = name;
}

void EDMPart::SetVertSize(int count) {
	EDMPart::vertSize = count;
}

void EDMPart::SetVertices(std::vector<Point3> vertices) {
	EDMPart::vertices = vertices;
}

void EDMPart::SetNormals(std::vector<Point3> normals) {
	EDMPart::normals = normals;
}

void EDMPart::SetUVSize(int count) {
	EDMPart::uvSize = count;
}

void EDMPart::SetUVs(std::vector<UVVert> uvs) {
	EDMPart::uvs = uvs;
}

void EDMPart::SetIndicesSize(int count) {
	EDMPart::indicesSize = count;
}

void EDMPart::SetIndices(std::vector<Int3> indices) {
	EDMPart::indices = indices;
}

void EDMPart::SetMesh(Mesh mesh) {
	EDMPart::mesh = mesh;
}

std::wstring EDMPart::GetName() {
	return EDMPart::name;
}

int EDMPart::GetVertSize() {
	return EDMPart::vertSize;
}

std::vector<Point3> EDMPart::GetVertices() {
	return EDMPart::vertices;
}

std::vector<Point3> EDMPart::GetNormals() {
	return EDMPart::normals;
}

int EDMPart::GetUVSize() {
	return EDMPart::uvSize;
}

std::vector<UVVert> EDMPart::GetUVs() {
	return EDMPart::uvs;
}

int EDMPart::GetIndicesSize() {
	return EDMPart::indicesSize;
}

std::vector<Int3> EDMPart::GetIndices() {
	return EDMPart::indices;
}

Mesh EDMPart::GetMesh() {
	return EDMPart::mesh;
}

void EDMPart::ReadFromStream(FILE * stream) {
	std::wstring partName = std::wstring();
	partName = ReadString(stream, partName);
	name = partName;
	fread(&vertSize, sizeof(int), 1, stream);
	vertices = std::vector<Point3>(vertSize);
	for (int i = 0; i != vertSize; i++) {
		fread(&vertices[i].x, sizeof(float), 1, stream);
		fread(&vertices[i].y, sizeof(float), 1, stream);
		fread(&vertices[i].z, sizeof(float), 1, stream);
	}
	normals = std::vector<Point3>(vertSize);
	for (int i = 0; i != vertSize; i++) {
		fread(&normals[i].x, sizeof(float), 1, stream);
		fread(&normals[i].y, sizeof(float), 1, stream);
		fread(&normals[i].z, sizeof(float), 1, stream);
	}

	fread(&uvSize, sizeof(int), 1, stream);
	uvs = std::vector<UVVert>(uvSize);
	for (int i = 0; i != uvSize; i++) {
		fread(&uvs[i].x, sizeof(float), 1, stream);
		fread(&uvs[i].y, sizeof(float), 1, stream);
	}

	fread(&indicesSize, sizeof(int), 1, stream);
	indices = std::vector<Int3>(indicesSize);
	for (int i = 0; i != indicesSize; i++) {
		fread(&indices[i].i1, sizeof(int), 1, stream);
		fread(&indices[i].i2, sizeof(int), 1, stream);
		fread(&indices[i].i3, sizeof(int), 1, stream);

		indices[i].i1 -= 1;
		indices[i].i2 -= 1;
		indices[i].i3 -= 1;
	}

	mesh = Mesh();

	mesh.setNumVerts(vertSize);
	for (int i = 0; i != mesh.numVerts; i++) {
		mesh.setVert(i, vertices[i]);
	}

	mesh.SpecifyNormals();
	MeshNormalSpec *normalSpec = mesh.GetSpecifiedNormals();
	normalSpec->ClearNormals();
	normalSpec->SetNumNormals(mesh.numVerts);
	for (int i = 0; i != mesh.numVerts; i++) {
		normalSpec->Normal(i) = normals[i];
		normalSpec->SetNormalExplicit(i, true);
	}

	mesh.setNumMaps(2);
	mesh.setMapSupport(1, true);
	MeshMap &map = mesh.Map(1);
	map.setNumVerts(vertSize);

	for (int i = 0; i != map.getNumVerts(); i++) {
		map.tv[i].x = uvs[i].x;
		map.tv[i].y = uvs[i].y;
		map.tv[i].z = 0.0f;
	}

	mesh.setNumFaces(indicesSize);
	map.setNumFaces(indicesSize);
	normalSpec->SetNumFaces(indicesSize);

	for (int i = 0; i != mesh.numFaces; i++) {
		mesh.faces[i].setVerts(indices[i].i1, indices[i].i2, indices[i].i3);
		mesh.faces[i].setMatID(1);
		mesh.faces[i].setEdgeVisFlags(1, 1, 1);
		normalSpec->Face(i).SpecifyAll();
		normalSpec->Face(i).SetNormalID(0, indices[i].i1);
		normalSpec->Face(i).SetNormalID(1, indices[i].i2);
		normalSpec->Face(i).SetNormalID(2, indices[i].i3);
		map.tf[i].setTVerts(indices[i].i1, indices[i].i2, indices[i].i3);
	}

	mesh.InvalidateGeomCache();
	mesh.InvalidateTopologyCache();

}

EDMPart::EDMPart() {}
EDMPart::~EDMPart() {}

//===================================================
//		EDMSTRUCTURE
//===================================================
void EDMStructure::SetName(std::wstring name) {
	EDMStructure::name = name;
}

void EDMStructure::SetPartSize(int count) {
	EDMStructure::partSize = count;
}

void EDMStructure::SetParts(std::vector<EDMPart> parts) {
	EDMStructure::parts = parts;
}

std::wstring EDMStructure::GetName() {
	return name;
}

int EDMStructure::GetPartSize() {
	return partSize;
}

std::vector<EDMPart> EDMStructure::GetParts() {
	return parts;
}

void EDMStructure::ReadFromStream(FILE * stream) {

	std::wstring edmName = std::wstring();
	edmName = ReadString(stream, edmName);
	name = edmName;
	fread(&partSize, sizeof(int), 1, stream);
	parts = std::vector<EDMPart>(partSize);
	
	for (int i = 0; i != parts.size(); i++) {
		parts[i].ReadFromStream(stream);
	}
	fclose(stream);
}

EDMStructure::EDMStructure() {}
EDMStructure::~EDMStructure() {}
