Re-implementation of the **tree** command found in powershell / linux / unix for **pretty-printing directory hierarchies.**

Andrew Lock's article https://andrewlock.net/creating-an-ascii-art-tree-in-csharp/ helped me solve an error.

# Future Work
- Additional error handling
- Printing as the Node class is constructed, rather than after it has been fully constructed.

Example:

     sqlite-build-tables
        │  csc run command.txt
        │  Program.cs
        │  sqlite-build-tables.csproj
        │  SQLite.Interop.dll
        ├─Assemblies
        │       netstandard.dll
        │       System.Data.SQLite.dll
        ├─bin
        │    └─Debug
        │         └─netcoreapp3.0
        │                 sqlite-build-tables.deps.json
        │                 sqlite-build-tables.dll
        │                 sqlite-build-tables.exe
        │                 sqlite-build-tables.pdb
        │                 sqlite-build-tables.runtimeconfig.dev.json
        │                 sqlite-build-tables.runtimeconfig.json
        │                 System.Data.SQLite.dll
        ├─Databases
        │       4-player-database.db
        └─obj
             │  project.assets.json
             │  sqlite-build-tables.csproj.nuget.cache
             │  sqlite-build-tables.csproj.nuget.dgspec.json
             │  sqlite-build-tables.csproj.nuget.g.props
             │  sqlite-build-tables.csproj.nuget.g.targets
             └─Debug
                  ├─netcoreapp2.1
                  │       sqlite-build-tables.AssemblyInfo.cs
                  │       sqlite-build-tables.AssemblyInfoInputs.cache
                  │       sqlite-build-tables.assets.cache
                  │       sqlite-build-tables.csproj.FileListAbsolute.txt
                  │       sqlite-build-tables.csprojAssemblyReference.cache
                  └─netcoreapp3.0
                          sqlite-build-tables.AssemblyInfo.cs
                          sqlite-build-tables.AssemblyInfoInputs.cache
                          sqlite-build-tables.assets.cache
                          sqlite-build-tables.csproj.CopyComplete
                          sqlite-build-tables.csproj.FileListAbsolute.txt
                          sqlite-build-tables.csprojAssemblyReference.cache
                          sqlite-build-tables.dll
                          sqlite-build-tables.exe
                          sqlite-build-tables.pdb
   
