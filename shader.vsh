vs_1_1
dcl_position v0
dcl_color v1

; c0 - matrix
; c4 - R
; c5 - weight
; new_r = r*(w+R/|r|*(1-w))


dp3 r1, v0, v0			; r1.x = v0*v0
rsq r0.x, r1.x			; r0.x = 1/sqrt(r0.x)
mul r0.x, r0.x, c4.x	; r0.x = R/|r|
sge r1.x, r1.x, r1.x	; r1.x = 1
sub r1.x, r1.x, c5.x	; r1.x = 1-w
mul r0.x, r0.x, r1.x	; r0.x = R/|r|*(1-w)
add r0.x, r0.x, c5.x	; r0.x = w+R/|r|*(1-w)
rcp r0.x, r0.x			; r0.x = 1/(w+R/|r|*(1-w))

mov r2, v0
mul r2.w, r2.w, r0.x
m4x4 oPos, r2, c0
mov oD0, v1