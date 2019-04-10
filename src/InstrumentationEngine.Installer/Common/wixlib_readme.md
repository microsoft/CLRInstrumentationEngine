Wixlibs contain only Fragments. Referencing an element in the fragment (for example DirectoryRef) opens up the whole fragment and makes all its elements immediately available.
(See https://www.firegiant.com/wix/tutorial/upgrades-and-modularization/fragments/)

Wixproj projects with ProjectReference to wixlib projects are enough to link the output wixlib file during build.
If an external project wants to reference the wixlib, add this ItemGroup:

```
<ItemGroup>
    <WixLibrary Include="[Path to .wixlib file]" />
</ItemGroup>
```

The InstrumentationEngine.Fragment.wixlib files contain the fragments defined in Wixlib\directories.wxi and Wixlib\components.wxi.
Please see [InstrumentationEngine.Installer.wxs](../Package/InstrumentationEngine.Installer.wxs) for examples on how to reference the fragment elements provided by the wixlibs.