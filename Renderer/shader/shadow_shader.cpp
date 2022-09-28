#pragma once
#include "./shader.h"
#include "../core/pipeline.h"




// world space -> NDC space -> screen space -> fagment shader
//            mvp              
//                 裁剪        重心插值（修正）=> 片段的法向量，uv，z值
//                                              世界空间的坐标，光源坐标，相机坐标
void ShadowShader::vertex_shader() {
	for (int i = 0; i < attribute.vertexs.size(); i++) {
		gl.positions[i] = uniform.vp_mat * uniform.model_mat * vec4(attribute.vertexs[i]);
		varying.normals[i] = uniform.normal_mat * vec4(attribute.normals[i]);
	}
}


vec3 ShadowShader::fragment_shader() {
	vec3 color(0, 0, 0);
	color = pack(gl.FragCoord.z());

	return color;
}