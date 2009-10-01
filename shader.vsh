vs_1_1
dcl_position v0
dcl_color v1

;begin Sphere
	mul r0, v0, v0
	mov r1.x, r0.x
	add r1.x, r1.x, r0.y
	add r1.x, r1.x, r0.z

	;r1.x = 1/sqrt(v^2)
	rsq r1.x, r1.x
	;r1.x = weight/sqrt(v^2)
	mul r1.x, r1.x, c4.x
	;r1.x = sphereRadius * weight / sqrt(v^2)
	mul r1.x, r1.x, c5.x
	
	mov r1.w, r1.x
	sge r1.w, r1.w, r1.w
	mul r1.xyz, v0.xyz, r1.xxx
;end Sphere

;start pyramid
	;r0.x = 1 - weight
	sge r0.x, r0.x, r0.x
	add r0.x, r0.x, -c4.x
	
	;r2.x = 1/v0.w
	rcp r2.x, v0.w
	;r2.x = (1-weight)/v0.w
	mul r2.x, r2.x, r0.x
	
	mul r0.xyz, v0.xyz, r2.xxx
	sge r0.w, r0.w, r0.w
;end pyramid


add r0, r0, r1
sge r0.w, r0.w, r0.w
m4x4 oPos, r0, c0
mov oD0, v1