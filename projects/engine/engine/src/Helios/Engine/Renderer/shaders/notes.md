Use a single authoring language and a small cross‑compile toolchain so one source shader can target all three APIs. Two practical, widely used approaches:

Recommendation (best balance)
- Author in HLSL.
- Compile with dxc (DirectX Shader Compiler):
- Produce native DXIL for DirectX 12.
- Produce SPIR‑V for Vulkan (DXC supports a -spirv/-T path).
- Convert SPIR‑V to GLSL for OpenGL with SPIRV‑Cross. Pros: excellent tooling for D3D12, clean semantics, mature compiler (DXC). Conversions are reliable and commonly used in engines.

Alternative (currently used in the premake script)
- Author in GLSL, compile to SPIR‑V for Vulkan (glslangValidator), convert SPIR‑V to HLSL for DX12 with SPIRV‑Cross (less common; DX12 tooling favors HLSL).

Why not raw GLSL/HLSL only
- Vulkan’s native form is SPIR‑V; using SPIR‑V as the intermediate gives consistent reflection and optimization. OpenGL typically needs GLSL (or converted GLSL), and DX12 prefers HLSL/DXIL — so an intermediate + cross‑compiler is required anyway.

Toolchain you’ll want
- dxc (HLSL → DXIL / SPIR‑V)
- glslangValidator (GLSL → SPIR‑V) — optional
- SPIRV‑Cross (SPIR‑V → GLSL/HLSL)
- SPIRV‑Tools / SPIRV‑Reflect for validation and reflection

Practical tips
- Use explicit binding/layout semantics in HLSL (set/binding) so mappings are deterministic across backends.
- Keep shader interfaces simple and centralized (a header for bindings) to avoid cross‑backend mismatches.
- Test compiled outputs on each API early; automated build steps should produce all three targets.

Bottom line: pick HLSL + DXC + SPIRV‑Cross for the smoothest path when targeting Vulkan, OpenGL and DirectX12.

