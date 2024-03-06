DirectX12を使用したobjファイルのビューワー。  
Unityの画面を意識して作成しており、パースした3Dモデルを表示・変更できる。Phongの反射モデルを使用。

# 使い方
1. リポジトリをクローン / ファイルをダウンロード
2. 表示したいobjファイルを
```
/model/
```
に追加。同名のmtlファイルや使用するテクスチャ（pngファイルのみ）もここに追加。  
※立方体と平面ポリゴンファイルは予め入れている。

3. 実行
```
/x64/Debug/OBJFileViewer.exe
```
をダブルクリック


※画面/操作説明については、**/OBJFileViewer_説明.pdf**を参照されたし。


※WindowsとDirectX12がインストールされている必要がある。  
※ファイルパスに日本語が入っていると実行ができない。  
※Microsoft Visual C++のインストールが必要である。


# 機能
- 動的にobjファイルやmtlファイルをパースし、3Dモデルの描画を行う。
- 複数の3Dモデルを表示することができる。
- 3Dモデルの移動回転拡大縮小ができる。
- 複数の3Dモデルの表示が可能。モデルの削除、複製もできる。
- マテリアルの実装が出来ており、Ambient光、Diffuse光、Specular光の変更やテクスチャなどの変更ができる。
- ディレクションライトが照射されており、ライトの方向や色の変更もできる。　　
- カメラの移動回転、Fovの変更、視錐台の変更もできる。

パースされた3Dモデルは、Modelインスタンスとして生成される。また、マテリアル毎にSubModelインスタンスが生成される。  

# 作成環境
- 言語：C++(14), HLSL
- API：DirectX12
- IDE：Visual Studio 2019(v142)
- WindowsSDK：v10.0.22000.0
- OS：Windows11
- ライブラリ：[DirectXTex](https://github.com/microsoft/DirectXTex)


- CPU：Intel Core i7-12700
- GPU：NVIDIA GeForce RTX 3060

# 参考にしたもの
- [Microsoft リファレンス](https://learn.microsoft.com/ja-jp/training/)  
    DirectX12の関数の扱い方、WindowsAPIの扱い方について
- DirectX12の魔導書 3Dレンダリングの基礎からMMDモデルを躍らせるまで, 川野竜一, 翔泳社  
    DirectX12の概要・扱い方、ポリゴンの表示やマテリアルの実装
- HLSLシェーダーの魔導書 シェーディングの基礎からレイトレーシングまで, 清原隆行, 翔泳社  
    CGの概念、HLSLについて
- [もんしょの巣穴 DirectXの話](https://sites.google.com/site/monshonosuana/directx%E3%81%AE%E8%A9%B1)  
    DirectX12の概念について
- [ぬの部屋（仮） Win32APIでウィンドウをカプセル化](https://suzulang.com/win32api%E3%81%A7%E3%82%A6%E3%82%A3%E3%83%B3%E3%83%89%E3%82%A6%E3%82%92%E3%82%AB%E3%83%97%E3%82%BB%E3%83%AB%E5%8C%96/)  
    ウィンドウプロシージャ関数の継承について
- [EternalWindows](https://eternalwindows.jp/index.html#windevelop)  
    ウィンドウの生成、ボタンやボックスの生成・変更など

# ファイルの簡単な説明
- main.cpp  
エントリーポイント。
- Application.cpp/h  
アプリケーションの全体的な管理を行う。
- BasicWindow.cpp/h  
Windowを生成、管理する抽象クラス。
- MainWindow.cpp/h  
描画したモデルを表示するMainWindowの生成・管理を行う。BasicWindowクラスを継承する。
- SourceWindow.cpp/h  
フォルダ内にあるobjファイルを表示したり、CameraやLigthの変更ができるSourceWindowの生成・管理を行う。BasicWindowクラスを継承する。
- ModelWindow.cpp/h  
表示しているModelをTreeViewで管理するModelWindow（UnityにおけるHierarchy Windowをイメージ）の生成・管理を行う。BasicWindowクラスを継承する。
- DetailWindow.cpp/h  
選択されているModelやSubModelの詳細を表示・変更できるDetailWindowの生成・管理を行う。BasicWindowクラスを継承する。
- ModelManager.cpp/h  
Modelの管理・生成・削除などを行う。
- Model.cpp/h  
頂点情報やSRT情報、SubModelを保持する。
- SubModel.cpp/h  
インデクス情報やMaterial情報、テクスチャ情報、SRT情報を保持する。
- Camera.cpp/h  
CameraとLightの情報を保持する。
- OBJFVLib.h
このプロジェクトで使用する構造体の定義を行う。  
- BasicTypes.hlsli  
BasicVSとBasicPSで使用する構造体の宣言を行う。
- BasicVS.hlsl  
使用するvertex shader。
- BasicPS.hlsl  
使用するpixel shader。
- model/  
使用する.objファイル、.mtlファイル、.pngファイルが入っているフォルダ。
- DirectXTex-main/
DirectXTexのライブラリ（DirectXTex.lib）が入っているフォルダ。D3D12ライブラリなども入っている。