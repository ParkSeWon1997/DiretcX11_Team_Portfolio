#include "BlackHole_Ring_Bill.h"
#include "GameInstance.h"

CBlackHole_Ring_Bill::CBlackHole_Ring_Bill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CBlackHole_Ring_Bill::CBlackHole_Ring_Bill(const CBlackHole_Ring_Bill& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CBlackHole_Ring_Bill::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlackHole_Ring_Bill::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_unique<BLACKHOLE_RING_BILL>(*((BLACKHOLE_RING_BILL*)pArg));

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;


	_matrix ParentMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ParentMat.r[3]);
	m_pTransformCom->Set_Scale(m_OwnDesc->vMinSize.x, m_OwnDesc->vMinSize.y, m_OwnDesc->vMinSize.z);
	return S_OK;
}

void CBlackHole_Ring_Bill::Priority_Tick(_float fTimeDelta)
{
}

void CBlackHole_Ring_Bill::Tick(_float fTimeDelta)
{
	if (Erase)
		return;
	m_AccTime += 0.016f;

	if (m_AccTime <= m_OwnDesc->fStartdelay && !m_SetDead)
	{
		_float t = m_AccTime / m_OwnDesc->fStartdelay;
		t = max(0.f, min(t, 1.f));
		_vector Minsize = XMLoadFloat3(&m_OwnDesc->vMinSize);
		_vector Maxsize = XMLoadFloat3(&m_OwnDesc->vMaxSize);
		_vector ResultSize = XMVectorLerp(Minsize, Maxsize, t);
		m_pTransformCom->Set_Scale(XMVectorGetX(ResultSize), XMVectorGetY(ResultSize), XMVectorGetZ(ResultSize));
	}

	if (m_SetDead)
	{
		if (m_AccTime > m_DeadDelay)
			Erase = true;

		_float t = m_AccTime / m_DeadDelay;
		t = max(0.f, min(t, 1.f));
		_vector Maxsize = XMLoadFloat3(&m_OwnDesc->vMaxSize);
		_vector Minsize = XMVectorZero();
		_vector ResultSize = XMVectorLerp(Maxsize, Minsize, t);
		m_pTransformCom->Set_Scale(XMVectorGetX(ResultSize), XMVectorGetY(ResultSize), XMVectorGetZ(ResultSize));
	}
	_matrix ParentMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ParentMat.r[3]);
}

void CBlackHole_Ring_Bill::Late_Tick(_float fTimeDelta)
{
	if (Erase)
		return;
	m_pTransformCom->BillBoard();
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CBlackHole_Ring_Bill::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		m_pShaderCom->Begin(36);
		m_pModelCom->Render(i);

	}
	return S_OK;
}

HRESULT CBlackHole_Ring_Bill::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		m_pShaderCom->Begin(37);
		m_pModelCom->Render(i);

	}
	return S_OK;
}


void CBlackHole_Ring_Bill::Set_BlackHole_Dead()
{
	m_DeadDelay = m_AccTime + m_OwnDesc->fStartdelay;
	m_SetDead = true;
}



HRESULT CBlackHole_Ring_Bill::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_BlackHoleRing2"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlackHole_Ring_Bill::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurTime", &m_AccTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &m_OwnDesc->fUVSpeed, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

HRESULT CBlackHole_Ring_Bill::Bind_BloomResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurTime", &m_AccTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->vBloomColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomPower", &m_OwnDesc->fBloomPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &m_OwnDesc->fUVSpeed, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}



CBlackHole_Ring_Bill* CBlackHole_Ring_Bill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlackHole_Ring_Bill* pInstance = new CBlackHole_Ring_Bill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBlackHole_Ring_Bill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlackHole_Ring_Bill::Clone(void* pArg)
{
	CBlackHole_Ring_Bill* pInstance = new CBlackHole_Ring_Bill(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBlackHole_Ring_Bill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlackHole_Ring_Bill::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
