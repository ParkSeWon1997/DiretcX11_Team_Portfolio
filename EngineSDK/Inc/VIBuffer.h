#pragma once

#include "Component.h"

/* 1. Vertex Buffer + Index Buffer */
/* 2. 정점와 인덱스의 버퍼(공간)을 생성하는 작업을 수행하낟. */
/* 3. 각기 다른 형태의 모델들응ㄹ 구성한다ㅏ(자식클래스로 etc. Rect, Cube, Model.) */

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();
	virtual HRESULT Render(ID3D11DeviceContext* pDeferredContext);

public:
	virtual float Compute_Height(const XMFLOAT3& vLocalPos) { return 0.f; }

public:
	virtual HRESULT Bind_Buffers();
	virtual HRESULT Bind_Buffers(ID3D11DeviceContext* pDeferredContext);

protected:
	ID3D11Buffer*				m_pVB = { nullptr };
	ID3D11Buffer*				m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC			m_BufferDesc = {};
	D3D11_SUBRESOURCE_DATA		m_InitialData = {};
	D3D11_SUBRESOURCE_DATA		m_InitialPoint = {};

	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology = {};
	DXGI_FORMAT					m_iIndexFormat = {};
	_uint						m_iNumVertexBuffers = { 0 };
	_uint						m_iVertexStride = { 0 };
	_uint						m_iNumVertices = { 0 };
	_uint						m_iIndexStride = { 0 };
	_uint						m_iNumIndices = { 0 };
	_uint						m_iNumPrimitive = { 0 };

	XMFLOAT4*					m_pVertexPositions = { nullptr };

protected:
	HRESULT Create_Buffer(ID3D11Buffer** ppVIBuffer);
	HRESULT Create_BufferZ(ID3D11Buffer** ppVIBuffer, D3D11_BUFFER_DESC* meshbuffer);

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END