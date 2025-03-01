#include "GroundSlash.h"
#include "GameInstance.h"

CGroundSlash::CGroundSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CGroundSlash::CGroundSlash(const CGroundSlash& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CGroundSlash::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGroundSlash::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	m_OwnDesc = make_shared<GROUNDSLASH>(*((GROUNDSLASH*)pArg));

	GAMEOBJECT_DESC Objdesc{};
	Objdesc.fSpeedPerSec = m_OwnDesc->fSpeed;
	if (FAILED(__super::Initialize(&Objdesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	_matrix WorldMat = XMMatrixIdentity();
	WorldMat.r[3] = XMLoadFloat4(&m_OwnDesc->vStartPos);
	WorldMat.r[2] = XMVector4Normalize(XMLoadFloat4(&m_OwnDesc->vDirection)); //Look
	WorldMat.r[0] = XMVector3Normalize(XMVector3Cross(WorldMat.r[2], XMVectorSet(0.f, 1.f, 0.f, 0.f))); 
	WorldMat.r[1] = XMVector3Normalize(XMVector3Cross(WorldMat.r[2], WorldMat.r[0]));
	m_pTransformCom->Set_WorldMatrix(WorldMat);

	m_pTransformCom->Set_Scale(m_OwnDesc->vStartSize.x, m_OwnDesc->vStartSize.y, m_OwnDesc->vStartSize.z);
	return S_OK;
}

void CGroundSlash::Priority_Tick(_float fTimeDelta)
{
}

void CGroundSlash::Tick(_float fTimeDelta)
{
	if (m_fCurLifeTime < m_OwnDesc->fThreadRatio)
	{
		m_fCurLifeTime += fTimeDelta * m_OwnDesc->fSlowStrength;
		m_pTransformCom->Go_Straight(fTimeDelta * m_OwnDesc->fSlowStrength);
		float t = m_fLifeTimeRatio / m_OwnDesc->fThreadRatio;
		_vector OrgSize = XMLoadFloat3(&m_OwnDesc->vStartSize);
		_vector EndSize = XMLoadFloat3(&m_OwnDesc->vEndSize);
		_vector ResultSize = XMVectorLerp(OrgSize, EndSize, t);
		m_pTransformCom->Set_Scale(ResultSize.m128_f32[0], ResultSize.m128_f32[1], ResultSize.m128_f32[2]);
	}
	else
	{
		m_fCurLifeTime += fTimeDelta;
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
		
	
	if (m_fCurLifeTime >= m_OwnDesc->fMaxLifeTime)
	{
		m_fCurLifeTime = m_OwnDesc->fMaxLifeTime;
		m_pGameInstance->Erase(this);
	}
	m_fLifeTimeRatio = m_fCurLifeTime / m_OwnDesc->fMaxLifeTime;
	m_fLifeTimeRatio = max(0.f, min(m_fLifeTimeRatio, 1.f));
}

void CGroundSlash::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CGroundSlash::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(31);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CGroundSlash::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(32);
		m_pModelCom->Render(i);
	}
	return S_OK;
}


HRESULT CGroundSlash::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GroundSlash"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGroundSlash::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->fColor, sizeof(_float3))))
		return E_FAIL;


	return S_OK;
}

HRESULT CGroundSlash::Bind_BloomResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomPower", &m_OwnDesc->fBloomPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->BloomColor, sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

CGroundSlash* CGroundSlash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGroundSlash* pInstance = new CGroundSlash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CGroundSlash");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CGroundSlash::Clone(void* pArg)
{
	CGroundSlash* pInstance = new CGroundSlash(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CGroundSlash");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGroundSlash::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
