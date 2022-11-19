#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <lm.h>

#include <iostream>

#include <napi.h>

std::string wstrtostr(const std::wstring &wstr)
{
    std::string strTo;
    int l = wstr.size() * 4;
    char *szTo = new char[l + 1];
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, szTo, l, NULL, NULL);
    strTo = szTo;
    delete[] szTo;
    return strTo;
}

Napi::Array
getAllUsers(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::Array resultArray = Napi::Array::New(env);
    int count = 0;
    LPUSER_INFO_0 pBuf = NULL;
    LPUSER_INFO_0 pTmpBuf;
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i;
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus;
    LPTSTR pszServerName = NULL;
    do
    {
        nStatus = NetUserEnum((LPCWSTR)pszServerName,
                              dwLevel,
                              FILTER_NORMAL_ACCOUNT,
                              (LPBYTE *)&pBuf,
                              dwPrefMaxLen,
                              &dwEntriesRead,
                              &dwTotalEntries,
                              &dwResumeHandle);
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
        {
            if ((pTmpBuf = pBuf) != NULL)
            {
                for (i = 0; (i < dwEntriesRead); i++)
                {
                    assert(pTmpBuf != NULL);
                    if (pTmpBuf == NULL)
                        break;
                    LPWSTR username = pTmpBuf->usri0_name;
                    resultArray.Set(Napi::Number::New(env, count++), Napi::String::New(env, wstrtostr(username)));
                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        }
        if (pBuf != NULL)
        {
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }
    } while (nStatus == ERROR_MORE_DATA);
    if (pBuf != NULL)
        NetApiBufferFree(pBuf);
    return resultArray;
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "getAllUsers"),
                Napi::Function::New(env, getAllUsers));
    return exports;
}

NODE_API_MODULE(getAllUsers, Init)