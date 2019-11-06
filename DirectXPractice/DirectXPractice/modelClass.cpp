#include "stdafx.h"
#include "textureArrayClass.h"
#include "modelClass.h"

ModelClass::ModelClass()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_iVertexCount = 0;
	m_iIndexCount = 0;
	m_pTextureArray = nullptr;
	m_pModel = nullptr;
}

ModelClass::ModelClass(const ModelClass&)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, WCHAR* modelFilename, WCHAR* textureFilename1, WCHAR* textureFilename2)
{
	if (!LoadModel(modelFilename))
	{
		return false;
	}

	if (!InitializeBuffers(device))
	{
		return false;
	}

	if (!LoadTextures(device, textureFilename1, textureFilename2))
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	ReleaseModel();
	ReleaseTextureArray();
	ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* immediateContext)
{
	RenderBuffers(immediateContext);
}

int ModelClass::GetIndexCount()
{
	return m_iIndexCount;
}

ID3D11ShaderResourceView** ModelClass::GetTextureArray()
{
	return  m_pTextureArray->GetTextureArray();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	// 정점 배열을 만듬.
	VertexType* vertices = new VertexType[m_iVertexCount];
	if (!vertices)
	{
		return false;
	}

	for (int i = 0; i < m_iVertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_pModel[i].x, m_pModel[i].y, m_pModel[i].z);
		vertices[i].texture = XMFLOAT2(m_pModel[i].tu, m_pModel[i].tv);
		vertices[i].normal = XMFLOAT3(m_pModel[i].nx, m_pModel[i].ny, m_pModel[i].nz);
	}

	// 정점 버퍼 구조체 설정.
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_iVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// SubResource 구조에 정점 데이터에 대한 포인터 제공.
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼 만듬.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer)))
	{
		return false;
	}

	// 인덱스 배열 생성.
	UINT* indices = new UINT[m_iIndexCount];
	if (!indices)
	{
		return false;
	}

	for (int i = 0; i < m_iIndexCount; i++)
	{
		indices[i] = i;
	}

	// 인덱스 버퍼 구조체 설정.
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * m_iIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// SubResource 구조체에 인덱스 버퍼 대한 포인터 제공.
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼 생성.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer)))
	{
		return false;
	}

	delete[] indices;
	indices = nullptr;

	delete[] vertices;
	vertices = nullptr;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* immediateContext)
{
	// 정점 버퍼의 단위와 오프셋 설정.
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	// Input Assembler에 버텍스 버퍼용 자원으로 활성화.
	immediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	// Input Assembler에 인덱스 버퍼용 자원으로 활성화.
	immediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Input Assembler에 정점 버퍼로 그릴 기본형을 설정.
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelClass::LoadTextures(ID3D11Device* device, WCHAR* texturename1, WCHAR* texturename2)
{
	m_pTextureArray= new TextureArrayClass;
	if (!m_pTextureArray)
	{
		return false;
	}

	if (!m_pTextureArray->Initialize(device, texturename1, texturename2))
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTextureArray()
{
	m_pTextureArray->Shutdown();
}

bool ModelClass::LoadModel(WCHAR* modelFilename)
{
	using namespace std;

	ifstream fin;
	fin.open(modelFilename);

	if (fin.fail())
	{
		return false;
	}

	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	fin >> m_iVertexCount;

	m_iIndexCount = m_iVertexCount;

	m_pModel = new ModelType[m_iVertexCount];
	if (!m_pModel)
	{
		return false;
	}

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	for (int i = 0; i < m_iVertexCount; i++)
	{
		fin >> m_pModel[i].x >> m_pModel[i].y >> m_pModel[i].z;
		fin >> m_pModel[i].tu >> m_pModel[i].tv;
		fin >> m_pModel[i].nx >> m_pModel[i].ny >> m_pModel[i].nz;
	}

	fin.close();

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_pModel)
	{
		delete[] m_pModel;
		m_pModel = nullptr;
	}
}

void ModelClass::CalculateModelVectors()
{
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;

	// 모델의 면 수를 계산.
	int faceCount = m_iVertexCount / 3;

	int index = 0;

	for (int i = 0; i < faceCount; i++)
	{
		vertex1.x = m_pModel[index].x;
		vertex1.y = m_pModel[index].y;
		vertex1.z = m_pModel[index].z;
		vertex1.tu = m_pModel[index].tu;
		vertex1.tv = m_pModel[index].tv;
		vertex1.nx = m_pModel[index].nx;
		vertex1.ny = m_pModel[index].ny;
		vertex1.nz = m_pModel[index].nz;
		index++;

		vertex2.x = m_pModel[index].x;
		vertex2.y = m_pModel[index].y;
		vertex2.z = m_pModel[index].z;
		vertex2.tu = m_pModel[index].tu;
		vertex2.tv = m_pModel[index].tv;
		vertex2.nx = m_pModel[index].nx;
		vertex2.ny = m_pModel[index].ny;
		vertex2.nz = m_pModel[index].nz;
		index++;

		vertex3.x = m_pModel[index].x;
		vertex3.y = m_pModel[index].y;
		vertex3.z = m_pModel[index].z;
		vertex3.tu = m_pModel[index].tu;
		vertex3.tv = m_pModel[index].tv;
		vertex3.nx = m_pModel[index].nx;
		vertex3.ny = m_pModel[index].ny;
		vertex3.nz = m_pModel[index].nz;
		index++;

		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);
		CalculateNormal(tangent, binormal, normal);

		m_pModel[index - 1].nx = normal.x;
		m_pModel[index - 1].ny = normal.y;
		m_pModel[index - 1].nz = normal.z;
		m_pModel[index - 1].tx = tangent.x;
		m_pModel[index - 1].ty = tangent.y;
		m_pModel[index - 1].tz = tangent.z;
		m_pModel[index - 1].bx = binormal.x;
		m_pModel[index - 1].by = binormal.y;
		m_pModel[index - 1].bz = binormal.z;

		m_pModel[index - 2].nx = normal.x;
		m_pModel[index - 2].ny = normal.y;
		m_pModel[index - 2].nz = normal.z;
		m_pModel[index - 2].tx = tangent.x;
		m_pModel[index - 2].ty = tangent.y;
		m_pModel[index - 2].tz = tangent.z;
		m_pModel[index - 2].bx = binormal.x;
		m_pModel[index - 2].by = binormal.y;
		m_pModel[index - 2].bz = binormal.z;

		m_pModel[index - 3].nx = normal.x;
		m_pModel[index - 3].ny = normal.y;
		m_pModel[index - 3].nz = normal.z;
		m_pModel[index - 3].tx = tangent.x;
		m_pModel[index - 3].ty = tangent.y;
		m_pModel[index - 3].tz = tangent.z;
		m_pModel[index - 3].bx = binormal.x;
		m_pModel[index - 3].by = binormal.y;
		m_pModel[index - 3].bz = binormal.z;
	}
}

void ModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangnet, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];

	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[2] = vertex3.tu - vertex1.tu;
	tvVector[2] = vertex3.tv - vertex1.tv;

	float den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	tangnet.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangnet.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangnet.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	float length = sqrt((tangnet.x * tangnet.x) + (tangnet.y * tangnet.y) + (tangnet.z * tangnet.z));

	tangnet.x = tangnet.x / length;
	tangnet.y = tangnet.y / length;
	tangnet.z = tangnet.z / length;

	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;
}

void ModelClass::CalculateNormal(VectorType tangnet, VectorType binormal, VectorType& normal)
{
	normal.x = (tangnet.y * binormal.z) - (tangnet.z * binormal.y);
	normal.y = (tangnet.z * binormal.x) - (tangnet.x * binormal.z);
	normal.z = (tangnet.x * binormal.y) - (tangnet.y * binormal.x);

	float length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;
}
