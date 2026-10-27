#include "StdAfx.h"
#include "CameraFirstEye.h"
#include "../xrEngine/xr_level_controller.h"
#include "../xrEngine/xr_object.h"
#include "object_broker.h"
#include "../Include/xrRender/Kinematics.h"

// CAM_1
CCameraFirstEye::CCameraFirstEye(CObject* p, u32 flags ) : CCameraBase(p, flags)
{
}

CCameraFirstEye::~CCameraFirstEye()
{
}

void CCameraFirstEye::SetBoneAttachment(LPCSTR bone_name)
{
	if (bone_name && bone_name[0])
	{
		m_bone_name				= bone_name;
		m_bone_id				= BI_NONE;
		m_bone_attach_enabled	= true;
	}
	else
	{
		m_bone_name				= "";
		m_bone_id				= BI_NONE;
		m_bone_attach_enabled	= false;
	}
}

void CCameraFirstEye::Load(const char* section)
{
	inherited::Load		(section);
	style				= csFirstEye;
}

void CCameraFirstEye::save(NET_Packet& packet) 
{ 
	save_data(pitch, packet); 
}

void CCameraFirstEye::load(IReader& packet)
{
	load_data(pitch, packet); 
}

void CCameraFirstEye::Update(Fvector& point, Fvector& noise_dangle, bool force_update_pos)
{
	inherited::Update(point, noise_dangle, force_update_pos);

	if (m_Flags.is(flRelativeLink))
	{
		parent->XFORM().transform_dir(vDirection);
		parent->XFORM().transform_dir(vNormal);
		return;
	}

	if (!m_bone_attach_enabled || !m_bone_name.size() || !parent)
	{
		return;
	}

	IRenderVisual* v = parent->Visual();
	IKinematics* K = v ? v->dcast_PKinematics() : nullptr;
	if (!K)
	{
		return;
	}

	K->CalculateBones(TRUE);

	if (m_bone_id == BI_NONE)
	{
		m_bone_id = K->LL_BoneID(m_bone_name.c_str());

		if (m_bone_id == BI_NONE)
		{
			m_bone_attach_enabled = false;
			return;
		}
	}

	const Fmatrix& bone_local = K->LL_GetBoneInstance(m_bone_id).mTransform;
	Fmatrix bone_world;
	bone_world.mul_43(parent->XFORM(), bone_local);

	Fvector cam_pos = bone_world.c;
	if (!m_cam_position.similar(zero_vel, EPS_L))
	{
		cam_pos.add(Fvector().mul(parent->XFORM().i, m_cam_position.x));
		cam_pos.add(Fvector().mul(parent->XFORM().j, m_cam_position.y));
		cam_pos.add(Fvector().mul(parent->XFORM().k, m_cam_position.z));
	}
	vPosition.set(cam_pos);
}

void CCameraFirstEye::Move( int cmd, float val, float factor )
{
	if (bClampPitch)
	{
		while (pitch < lim_pitch[0])
			pitch += PI_MUL_2;
		while (pitch > lim_pitch[1])
			pitch -= PI_MUL_2;
	};
	switch (cmd){
	case kDOWN:		pitch	-= val?val:(rot_speed.y*Device.fTimeDelta/factor);	break;
	case kUP:		pitch	+= val?val:(rot_speed.y*Device.fTimeDelta/factor);	break;
	case kLEFT:		yaw		-= val?val:(rot_speed.x*Device.fTimeDelta/factor);	break;
	case kRIGHT:	yaw		+= val?val:(rot_speed.x*Device.fTimeDelta/factor);	break;
	}
	if (bClampYaw)		clamp(yaw,lim_yaw[0],lim_yaw[1]);
	if (bClampPitch)	clamp(pitch,lim_pitch[0],lim_pitch[1]);
}

void CCameraFirstEye::OnActivate( CCameraBase* old_cam )
{
	m_bone_id = BI_NONE;

	if (old_cam) {
		if (m_Flags.is(flRelativeLink) == old_cam->m_Flags.is(flRelativeLink))
			yaw = (old_cam)->yaw;

		if (m_Flags.is(flKeepPitch))
			pitch = (old_cam)->pitch;
	}
}