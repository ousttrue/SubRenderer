#pragma once
#include <d3d11.h>
#include <Unknwn.h>


MIDL_INTERFACE("3AB84C8F-A1CC-4CE1-A115-D3DF817C9A28")
IDeviceManager : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetDevice(ID3D11Device **ppDevice)=0;
};

// DeviceResources �����L���Ă���A�v���P�[�V�������A�f�o�C�X������ꂽ�Ƃ��܂��͍쐬���ꂽ�Ƃ��ɒʒm���󂯂邽�߂̃C���^�[�t�F�C�X��񋟂��܂��B
struct IDeviceNotify
{
    virtual void OnDeviceLost() = 0;
    virtual void OnDeviceRestored() = 0;
};
