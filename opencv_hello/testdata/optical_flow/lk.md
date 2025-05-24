# Lucas-Kanade

Originally was designed in dense optical flow, but can be used in sparse optical flow.

## Example
<img src="/testdata/optical_flow/frame_0.jpg" width="100" height="200"/>
<img src="/testdata/optical_flow/frame_27.jpg" width="100" height="200"/>
<img src="/optical_flow/images/lk-optical-flow.png" width="200" height="400"/>

Assumptions:
* Brightness consistency
* Temporal persistance ("small movements")
* Spatial coherence (neighbors stay neighbors)