#ifndef __CAMERA_FE_H__
#define __CAMERA_FE_H__

#include "../xrEngine/CameraBase.h"

class CCameraFirstEye : public CCameraBase
{
	using inherited = CCameraBase;
public:
					CCameraFirstEye	( CObject* p, u32 flags=0);
	virtual			~CCameraFirstEye( );

	virtual void	Load			(const char* section);
	virtual void	Move			( int cmd, float val=0, float factor=1.0f );

	virtual	void	OnActivate		( CCameraBase* old_cam );
	virtual void	Update			( Fvector& point, Fvector& noise_angle, bool force_update_pos );

	virtual float	GetWorldYaw		( )	{ return -yaw;	};
	virtual float	GetWorldPitch	( )	{ return pitch; };



			void save(NET_Packet& output_packet) override;
			void load(IReader& input_packet) override;

	void SetBoneAttachment	(LPCSTR bone_name);
	void SetCamPosition		(const Fvector& offset) { m_cam_position = offset; }
	bool IsBoneAttached		() const				{ return m_bone_attach_enabled; }
	void ResetBoneCache		()						{ m_bone_id = BI_NONE; }

private:
	shared_str	m_bone_name;
	u16			m_bone_id				= BI_NONE;
	bool		m_bone_attach_enabled	= false;
	Fvector		m_cam_position			= { 0.f, 0.f, 0.f };
};

#endif // __CAMERALOOK_H__
