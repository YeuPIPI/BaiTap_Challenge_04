#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#include <atlcomcli.h>

#pragma comment(lib, "wbemuuid.lib")
#include "CreationEvent.h"

class EventSink : public IWbemObjectSink {
    friend void CreationEvent::registerCreationCallback(TNotificationFunc callback);

    CComPtr<IWbemServices> pSvc;
    CComPtr<IWbemObjectSink> pStubSink;
    LONG m_IRef;
    CreationEvent::TNotificationFunc m_callback;

public:
    EventSink(CreationEvent::TNotificationFunc callback) :m_IRef(0), m_callback(callback) {}
    ~EventSink() {
    }

    virtual ULONG STDMETHODCALLTYPE AddRef() {
        return InterlockedIncrement(&m_IRef);
    }

    virtual ULONG STDMETHODCALLTYPE Release() {
        LONG IRef = InterlockedDecrement(&m_IRef);
        if (IRef == 0)
            delete this;
        return IRef;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) {
        if (riid == IID_IUnknown || riid == IID_IWbemObjectSink) {
            *ppv = (IWbemObjectSink*)this;
            AddRef();
            return WBEM_S_NO_ERROR;
        }
        else return E_NOINTERFACE;
    }

    virtual HRESULT STDMETHODCALLTYPE Indicate(
        LONG lObjectCount,
        IWbemClassObject __RPC_FAR* __RPC_FAR* apObjArray
    ) {
        m_callback();
        /* Unregister event sink */
        pSvc->CancelAsyncCall(pStubSink);
        return WBEM_S_NO_ERROR;
    }
    virtual HRESULT STDMETHODCALLTYPE SetStatus(LONG IFlags, HRESULT hResult, BSTR strParam, IWbemClassObject __RPC_FAR* pObjParam) {
        return WBEM_S_NO_ERROR;
    }
};

void CreationEvent::registerCreationCallback(TNotificationFunc callback) {
    CComPtr<IWbemLocator> pLoc;
    CoInitializeEx(0, COINIT_MULTITHREADED);
    HRESULT hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hres)) {
        cout << "Failed to create IWbemLocator object."
            << " Err code = 0x"
            << hex << hres << endl;
        throw std::exception("CreationEvent initialization failed");
    }
    CComPtr<EventSink> pSink(new EventSink(callback));

    hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSink->pSvc);
    if (FAILED(hres)) {
        cout << "Could not connect. Error code = 0x" << hex << hres << endl;
        throw std::exception("CreationEvent initialization failed");
    }
    hres = CoSetProxyBlanket(pSink->pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (FAILED(hres)) {
        cout << "Coult not set proxy blanket, Error code =0x" << hex << hres << endl;
        throw std::exception("CreationEvent initialization failed");
    }

    CComPtr<IUnsecuredApartment> pUnsecApp;
    hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL, CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment, (void**)&pUnsecApp);
    CComPtr<IUnknown> pStubUnk;
    pUnsecApp->CreateObjectStub(pSink, &pStubUnk);
    pStubUnk->QueryInterface(IID_IWbemObjectSink, (void**)&pSink->pStubSink);


    char buffer[512];
    sprintf_s(buffer, "SELECT * FROM __InstanceCreationEvent WITHIN 1 WHERE TargetInstance ISA 'Win32_Process' AND TargetInstance.Name = 'iexplore.exe'");

    hres = pSink->pSvc->ExecNotificationQueryAsync(_bstr_t("WQL"), _bstr_t(buffer), WBEM_FLAG_SEND_STATUS, NULL, pSink->pStubSink);

    if (FAILED(hres)) {
        cout << "ExecNotificationQueryAsync failed with = 0x" << hex << hres << endl;
        throw std::exception("CreationEvent initialization failed");
    }
}

void k() { cout << "KKKKK " << endl; }

int main() {
    CreationEvent::registerCreationCallback(k);
    cin.get();
}