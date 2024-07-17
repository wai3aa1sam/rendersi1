
# rendersi1
rendersi1 is a personal renderer for demo purpose, easy to use and maintain

**Table of Contents**
- Requirement
- Build
- Demos
- Features
- Architecture / Design
- Roadmap
- References

## Requirement

- Visual Studio 2022 (C++17)
- Windows only
- Vulkan 1.3 (download from https://www.lunarg.com/vulkan-sdk/)
- CMake

## Build

```
script/generate_project/install_vcpkg_packages.bat
script/generate_project/gen_vs-2022.bat
build/rendersi1-x64-windows/rendersi1.sln
```

## Demos

### Forward Plus Rendering

| fwdp | light heatmap | with frustum culling |
|---|---|---|
|![](example/Test000/screenshot/forward_plus/fwdp.jpg)|![](example/Test000/screenshot/forward_plus/fwdp_heatmap.jpg)|![](example/Test000/screenshot/forward_plus/fwdp_frustum_culling.jpg)

### Render Graph

| graph visualization | profile with tracy |
|---|---|
|![](example/Test000/screenshot/render_graph/render_graph_vxgi.jpg)|![](example/Test000/screenshot/render_graph/render_graph_tracy.jpg)|

### Voxel Cone Tracing (wip)

| voxelization | voxel wireframe | clipmap |
|---|---|---|
|![](example/Test000/screenshot/voxel_cone_tracing/vxgi_voxel.jpg)|![](example/Test000/screenshot/voxel_cone_tracing/vxgi_voxel_wireframe.jpg)|![](example/Test000/screenshot/voxel_cone_tracing/vxgi_clipmap.jpg)|

### Physical Based Rendering

| pbr | pbribl |
|---|---|
|![](example/Test000/screenshot/physical_based_rendering/pbr.jpg)|![](example/Test000/screenshot/physical_based_rendering/pbribl.jpg)|

### Cascaded Shadow Maps (wip)

| csm | cascade level |
|---|---|
|![](example/Test000/screenshot/cascaded_shadow_maps/csm.jpg)|![](example/Test000/screenshot/cascaded_shadow_maps/csm_cascade_level.jpg)|

## Features

- bindless rendering pipeline
- render pass feature
- shader hot reload
- ecs
- job system
- editor ui

## Architecture / Design

![](doc/architecture/architecture.jpg)

Work in progress to a multi-thread design. Support n frames in-flight, Sync only when the previous n frames has not yet completed. \
Main thread handle all game logic and record render commands, it could dispatch workload to worker threads. \
Render thread resolve those render commands to backend graphics api commands. It works as worker thread when there is no commands.

- pros
  - no lock is needed
  - easy to multi thread
  
- cons
  - memory usage will increase n times (multi buffer)

## Roadmap

- engine

  - multi thread design

- rendering

  - debug vxgi
  - atmospheric scattering
  - bloom
  - screen space reflection
  - cascaded shadow map
    - [ ] blend between cascade level
    - [ ] fix visual problem

- render api
  
  - enhance render graph
    - [ ] async compute
    - [ ] reorder
    - [ ] resource alias
    - [ ] dump graph with extra info (vk transition / our StateTrack)
  

## References

please go to [reference](doc/reference/reference.md)