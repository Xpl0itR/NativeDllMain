NativeDllMain [![NuGet](https://img.shields.io/nuget/vpre/NativeDllMain)](https://www.nuget.org/packages/NativeDllMain)
=============
A static library for NativeAOT projects, providing a native [DllMain](https://learn.microsoft.com/en-us/windows/win32/dlls/dllmain) implementation that safely invokes a managed entrypoint after the Windows loader lock is released.

This library is necessary because the dotnet runtime initialization code does not meet the [restrictions imposed by the Windows loader lock](https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices#general-best-practices),
making it impossible to implement DllMain directly in managed code.

NOTE: This functionality is only enabled when targeting `win-x64` or `win-x86`.


Usage
-----
1. Reference the [NativeDllMain NuGet package](https://www.nuget.org/packages/NativeDllMain) in your project.
2. Add the following method to your project:
```csharp
[UnmanagedCallersOnly(EntryPoint = "DllProcessAttach")]
public static void Main(nint hModule) // can be named anything you want as long as the unmanaged symbol is set to be "DllProcessAttach"
{
    /*
        Your entrypoint code here
    */
}
```
Now, when your DLL is loaded, [CreateThread](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createthread) is used to spawn a new thread for your managed entrypoint,
which will be suspended until after the loader lock is released.

### [QueueUserAPC](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-queueuserapc)
> If an application queues an APC before the thread begins running, the thread begins by calling the APC function.

For a DLL that is loaded via the **Import Address Table (IAT)**, such as a [proxy DLL](https://github.com/Xpl0itR/VersionShim),
DllMain is called before the main thread begins execution.
Therefore it's recommended to use QueueUserAPC to call your managed entrypoint, instead of CreateThread,
which prevents a race condition between the main thread and your managed entrypoint.

To enable this behaviour, add the following to a `<PropertyGroup>` in your csproj:
```xml
<DllMainUseQueueUserApc>true</DllMainUseQueueUserApc>
```
NOTE: If your DLL is loaded via [LoadLibrary](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibrarya) or similar,
you should continue to use CreateThread (the default behaviour), otherwise your managed entrypoint may never be called.


Building
--------
Essentially just replicate the [CI instructions](.github/workflows/release.yml).


License
-------
This project is subject to the terms of the [Mozilla Public License, v. 2.0](./LICENSE).