#include "OVR.h"
#include "IOculusRiftSupport.h"
#include "GL/gl.h"
#include "GL/glu.h"

class COculus: public IOculusRiftSupport
{
public:
	COculus();        
	~COculus();

	bool Start(InitError &initError);	
	bool GetSensorPos(CVector3D& pos, float& yaw, float& pitch, float& roll) const override;

	bool GetDesktopParams(int& wResolution, int& hResolution, int& displayX, int& displayY) override;

	void SetLeftEye(bool isLeftEye) override;
	void GetViewportParams(int& x, int& y, int& width, int& height) override;	
	void GetShaderParams(ShaderParams* params) override;

	void BeginRender() override;
	void EndRender() override;
	DistortionMesh* GetDistortionMesh() override;
	void DestroyDistortionMesh() override;

private:
	float* GetViewAdjustMatrix();
	float* GetProjectionMatrix();

    ovrHmd m_HMD;
	ovrEyeType m_eye;
	ovrEyeRenderDesc m_eyeRenderDesc[2];
	DistortionMesh* m_distortionMesh;

	OVR::Matrix4f m_viewAdjust;
	OVR::Matrix4f m_projMatrix;
};
 