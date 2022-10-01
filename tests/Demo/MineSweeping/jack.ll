
@Main_vtable = private global [14 x i64] zeroinitializer
@0 = private unnamed_addr constant [19 x i8] c"\22Insert Value i: \22\00", align 1
@1 = private unnamed_addr constant [19 x i8] c"\22Insert Value j: \22\00", align 1
@2 = private unnamed_addr constant [10 x i8] c"\22You Win\22\00", align 1
@3 = private unnamed_addr constant [11 x i8] c"\22You Lose\22\00", align 1

declare i32 @puts(i8*)

declare i32 @putchar(i32)

declare i32 @getchar()

declare i32 @rand()

define i32 @Main_randomNumber() {
entry:
  %x = alloca i32
  %0 = call i32 @rand()
  store i32 %0, i32* %x
  br label %whilecond

whilecond:                                        ; preds = %while, %entry
  %loadvalue = load i32, i32* %x
  %gt = icmp sgt i32 %loadvalue, 9
  br i1 %gt, label %while, label %next

while:                                            ; preds = %whilecond
  %loadvalue1 = load i32, i32* %x
  %div = sdiv i32 %loadvalue1, 10
  store i32 %div, i32* %x
  br label %whilecond

next:                                             ; preds = %whilecond
  %loadvalue2 = load i32, i32* %x
  ret i32 %loadvalue2
}

define i1 @Main_isWin([100 x i32] %Mask, i32 %num_mines) {
entry:
  %i = alloca i32
  %sum = alloca i32
  %Mask1 = alloca [100 x i32]
  store [100 x i32] %Mask, [100 x i32]* %Mask1
  %num_mines2 = alloca i32
  store i32 %num_mines, i32* %num_mines2
  store i32 0, i32* %i
  store i32 0, i32* %sum
  br label %whilecond

whilecond:                                        ; preds = %while, %entry
  %loadvalue = load i32, i32* %i
  %lt = icmp slt i32 %loadvalue, 100
  br i1 %lt, label %while, label %next

while:                                            ; preds = %whilecond
  %loadvalue3 = load i32, i32* %sum
  %loadvalue4 = load i32, i32* %i
  %array_member_addr = getelementptr [100 x i32], [100 x i32]* %Mask1, i32 0, i32 %loadvalue4
  %loadvalue5 = load i32, i32* %array_member_addr
  %add = add i32 %loadvalue3, %loadvalue5
  store i32 %add, i32* %sum
  %loadvalue6 = load i32, i32* %i
  %add7 = add i32 %loadvalue6, 1
  store i32 %add7, i32* %i
  br label %whilecond

next:                                             ; preds = %whilecond
  %loadvalue8 = load i32, i32* %sum
  %loadvalue9 = load i32, i32* %num_mines2
  %eq = icmp eq i32 %loadvalue8, %loadvalue9
  br i1 %eq, label %then, label %ifcont

then:                                             ; preds = %next
  ret i1 true
  br label %ifcont

ifcont:                                           ; preds = %then, %next
  ret i1 false
}

define i32 @Main_computeBoardPosition(i32 %i, i32 %j) {
entry:
  %result = alloca i32
  %i1 = alloca i32
  store i32 %i, i32* %i1
  %j2 = alloca i32
  store i32 %j, i32* %j2
  %loadvalue = load i32, i32* %j2
  %mul = mul i32 %loadvalue, 10
  %loadvalue3 = load i32, i32* %i1
  %add = add i32 %mul, %loadvalue3
  store i32 %add, i32* %result
  %loadvalue4 = load i32, i32* %result
  ret i32 %loadvalue4
}

define i32 @Main_generateInitialPosition() {
entry:
  %i = alloca i32
  %j = alloca i32
  %result = alloca i32
  %0 = call i32 @Main_randomNumber()
  store i32 %0, i32* %i
  %1 = call i32 @Main_randomNumber()
  store i32 %1, i32* %j
  %loadvalue = load i32, i32* %i
  %loadvalue1 = load i32, i32* %j
  %2 = call i32 @Main_computeBoardPosition(i32 %loadvalue, i32 %loadvalue1)
  store i32 %2, i32* %result
  %loadvalue2 = load i32, i32* %result
  ret i32 %loadvalue2
}

define i32 @Main_getUpPosition(i32 %i, i32 %j) {
entry:
  %up = alloca i32
  %i1 = alloca i32
  store i32 %i, i32* %i1
  %j2 = alloca i32
  store i32 %j, i32* %j2
  %loadvalue = load i32, i32* %j2
  %add = add i32 %loadvalue, 1
  store i32 %add, i32* %up
  %loadvalue3 = load i32, i32* %up
  %lt = icmp slt i32 %loadvalue3, 10
  br i1 %lt, label %then, label %ifcont

then:                                             ; preds = %entry
  %loadvalue4 = load i32, i32* %i1
  %loadvalue5 = load i32, i32* %up
  %0 = call i32 @Main_computeBoardPosition(i32 %loadvalue4, i32 %loadvalue5)
  ret i32 %0
  br label %ifcont

ifcont:                                           ; preds = %then, %entry
  ret i32 -1
}

define i32 @Main_getDownPosition(i32 %i, i32 %j) {
entry:
  %down = alloca i32
  %i1 = alloca i32
  store i32 %i, i32* %i1
  %j2 = alloca i32
  store i32 %j, i32* %j2
  %loadvalue = load i32, i32* %j2
  %sub = sub i32 %loadvalue, 1
  store i32 %sub, i32* %down
  %loadvalue3 = load i32, i32* %down
  %gt = icmp sgt i32 %loadvalue3, -1
  br i1 %gt, label %then, label %ifcont

then:                                             ; preds = %entry
  %loadvalue4 = load i32, i32* %i1
  %loadvalue5 = load i32, i32* %down
  %0 = call i32 @Main_computeBoardPosition(i32 %loadvalue4, i32 %loadvalue5)
  ret i32 %0
  br label %ifcont

ifcont:                                           ; preds = %then, %entry
  ret i32 -1
}

define i32 @Main_getLeftPosition(i32 %i, i32 %j) {
entry:
  %left = alloca i32
  %i1 = alloca i32
  store i32 %i, i32* %i1
  %j2 = alloca i32
  store i32 %j, i32* %j2
  %loadvalue = load i32, i32* %i1
  %sub = sub i32 %loadvalue, 1
  store i32 %sub, i32* %left
  %loadvalue3 = load i32, i32* %left
  %gt = icmp sgt i32 %loadvalue3, -1
  br i1 %gt, label %then, label %ifcont

then:                                             ; preds = %entry
  %loadvalue4 = load i32, i32* %left
  %loadvalue5 = load i32, i32* %j2
  %0 = call i32 @Main_computeBoardPosition(i32 %loadvalue4, i32 %loadvalue5)
  ret i32 %0
  br label %ifcont

ifcont:                                           ; preds = %then, %entry
  ret i32 -1
}

define i32 @Main_getRightPosition(i32 %i, i32 %j) {
entry:
  %right = alloca i32
  %i1 = alloca i32
  store i32 %i, i32* %i1
  %j2 = alloca i32
  store i32 %j, i32* %j2
  %loadvalue = load i32, i32* %i1
  %add = add i32 %loadvalue, 1
  store i32 %add, i32* %right
  %loadvalue3 = load i32, i32* %right
  %lt = icmp slt i32 %loadvalue3, 10
  br i1 %lt, label %then, label %ifcont

then:                                             ; preds = %entry
  %loadvalue4 = load i32, i32* %right
  %loadvalue5 = load i32, i32* %j2
  %0 = call i32 @Main_computeBoardPosition(i32 %loadvalue4, i32 %loadvalue5)
  ret i32 %0
  br label %ifcont

ifcont:                                           ; preds = %then, %entry
  ret i32 -1
}

define i32 @Main_getUpLeftPosition(i32 %i, i32 %j) {
entry:
  %ul_i = alloca i32
  %ul_j = alloca i32
  %i1 = alloca i32
  store i32 %i, i32* %i1
  %j2 = alloca i32
  store i32 %j, i32* %j2
  %loadvalue = load i32, i32* %i1
  %sub = sub i32 %loadvalue, 1
  store i32 %sub, i32* %ul_i
  %loadvalue3 = load i32, i32* %j2
  %add = add i32 %loadvalue3, 1
  store i32 %add, i32* %ul_j
  %loadvalue4 = load i32, i32* %ul_i
  %gt = icmp sgt i32 %loadvalue4, -1
  br i1 %gt, label %then, label %ifcont

then:                                             ; preds = %entry
  %loadvalue5 = load i32, i32* %ul_j
  %lt = icmp slt i32 %loadvalue5, 10
  br i1 %lt, label %then6, label %ifcont7

then6:                                            ; preds = %then
  %loadvalue8 = load i32, i32* %ul_i
  %loadvalue9 = load i32, i32* %ul_j
  %0 = call i32 @Main_computeBoardPosition(i32 %loadvalue8, i32 %loadvalue9)
  ret i32 %0
  br label %ifcont7

ifcont7:                                          ; preds = %then6, %then
  br label %ifcont

ifcont:                                           ; preds = %ifcont7, %entry
  ret i32 -1
}

define i32 @Main_getUpRightPosition(i32 %i, i32 %j) {
entry:
  %ur_i = alloca i32
  %ur_j = alloca i32
  %i1 = alloca i32
  store i32 %i, i32* %i1
  %j2 = alloca i32
  store i32 %j, i32* %j2
  %loadvalue = load i32, i32* %i1
  %add = add i32 %loadvalue, 1
  store i32 %add, i32* %ur_i
  %loadvalue3 = load i32, i32* %j2
  %add4 = add i32 %loadvalue3, 1
  store i32 %add4, i32* %ur_j
  %loadvalue5 = load i32, i32* %ur_i
  %lt = icmp slt i32 %loadvalue5, 10
  br i1 %lt, label %then, label %ifcont

then:                                             ; preds = %entry
  %loadvalue6 = load i32, i32* %ur_j
  %lt7 = icmp slt i32 %loadvalue6, 10
  br i1 %lt7, label %then8, label %ifcont9

then8:                                            ; preds = %then
  %loadvalue10 = load i32, i32* %ur_i
  %loadvalue11 = load i32, i32* %ur_j
  %0 = call i32 @Main_computeBoardPosition(i32 %loadvalue10, i32 %loadvalue11)
  ret i32 %0
  br label %ifcont9

ifcont9:                                          ; preds = %then8, %then
  br label %ifcont

ifcont:                                           ; preds = %ifcont9, %entry
  ret i32 -1
}

define i32 @Main_getDownLeftPosition(i32 %i, i32 %j) {
entry:
  %dl_i = alloca i32
  %dl_j = alloca i32
  %i1 = alloca i32
  store i32 %i, i32* %i1
  %j2 = alloca i32
  store i32 %j, i32* %j2
  %loadvalue = load i32, i32* %i1
  %sub = sub i32 %loadvalue, 1
  store i32 %sub, i32* %dl_i
  %loadvalue3 = load i32, i32* %j2
  %sub4 = sub i32 %loadvalue3, 1
  store i32 %sub4, i32* %dl_j
  %loadvalue5 = load i32, i32* %dl_i
  %gt = icmp sgt i32 %loadvalue5, -1
  br i1 %gt, label %then, label %ifcont

then:                                             ; preds = %entry
  %loadvalue6 = load i32, i32* %dl_j
  %gt7 = icmp sgt i32 %loadvalue6, -1
  br i1 %gt7, label %then8, label %ifcont9

then8:                                            ; preds = %then
  %loadvalue10 = load i32, i32* %dl_i
  %loadvalue11 = load i32, i32* %dl_j
  %0 = call i32 @Main_computeBoardPosition(i32 %loadvalue10, i32 %loadvalue11)
  ret i32 %0
  br label %ifcont9

ifcont9:                                          ; preds = %then8, %then
  br label %ifcont

ifcont:                                           ; preds = %ifcont9, %entry
  ret i32 -1
}

define i32 @Main_getDownRightPosition(i32 %i, i32 %j) {
entry:
  %dr_i = alloca i32
  %dr_j = alloca i32
  %i1 = alloca i32
  store i32 %i, i32* %i1
  %j2 = alloca i32
  store i32 %j, i32* %j2
  %loadvalue = load i32, i32* %i1
  %add = add i32 %loadvalue, 1
  store i32 %add, i32* %dr_i
  %loadvalue3 = load i32, i32* %j2
  %sub = sub i32 %loadvalue3, 1
  store i32 %sub, i32* %dr_j
  %loadvalue4 = load i32, i32* %dr_i
  %lt = icmp slt i32 %loadvalue4, 10
  br i1 %lt, label %then, label %ifcont

then:                                             ; preds = %entry
  %loadvalue5 = load i32, i32* %dr_j
  %gt = icmp sgt i32 %loadvalue5, -1
  br i1 %gt, label %then6, label %ifcont7

then6:                                            ; preds = %then
  %loadvalue8 = load i32, i32* %dr_i
  %loadvalue9 = load i32, i32* %dr_j
  %0 = call i32 @Main_computeBoardPosition(i32 %loadvalue8, i32 %loadvalue9)
  ret i32 %0
  br label %ifcont7

ifcont7:                                          ; preds = %then6, %then
  br label %ifcont

ifcont:                                           ; preds = %ifcont7, %entry
  ret i32 -1
}

define [100 x i32] @Main_BFS([100 x i32] %Board, [100 x i32] %Mask, i32 %i, i32 %j) {
entry:
  %curr = alloca i32
  %skip = alloca i1
  %Board1 = alloca [100 x i32]
  store [100 x i32] %Board, [100 x i32]* %Board1
  %Mask2 = alloca [100 x i32]
  store [100 x i32] %Mask, [100 x i32]* %Mask2
  %i3 = alloca i32
  store i32 %i, i32* %i3
  %j4 = alloca i32
  store i32 %j, i32* %j4
  store i1 false, i1* %skip
  %loadvalue = load i32, i32* %i3
  %gt = icmp sgt i32 %loadvalue, 9
  br i1 %gt, label %then, label %ifcont

then:                                             ; preds = %entry
  store i1 true, i1* %skip
  br label %ifcont

ifcont:                                           ; preds = %then, %entry
  %loadvalue5 = load i32, i32* %i3
  %lt = icmp slt i32 %loadvalue5, 0
  br i1 %lt, label %then6, label %ifcont7

then6:                                            ; preds = %ifcont
  store i1 true, i1* %skip
  br label %ifcont7

ifcont7:                                          ; preds = %then6, %ifcont
  %loadvalue8 = load i32, i32* %j4
  %gt9 = icmp sgt i32 %loadvalue8, 9
  br i1 %gt9, label %then10, label %ifcont11

then10:                                           ; preds = %ifcont7
  store i1 true, i1* %skip
  br label %ifcont11

ifcont11:                                         ; preds = %then10, %ifcont7
  %loadvalue12 = load i32, i32* %j4
  %lt13 = icmp slt i32 %loadvalue12, 0
  br i1 %lt13, label %then14, label %ifcont15

then14:                                           ; preds = %ifcont11
  store i1 true, i1* %skip
  br label %ifcont15

ifcont15:                                         ; preds = %then14, %ifcont11
  %loadvalue16 = load i1, i1* %skip
  %eq = icmp eq i1 %loadvalue16, false
  br i1 %eq, label %then17, label %ifcont18

then17:                                           ; preds = %ifcont15
  %loadvalue19 = load i32, i32* %i3
  %loadvalue20 = load i32, i32* %j4
  %0 = call i32 @Main_computeBoardPosition(i32 %loadvalue19, i32 %loadvalue20)
  store i32 %0, i32* %curr
  %loadvalue21 = load i32, i32* %curr
  %array_member_addr = getelementptr [100 x i32], [100 x i32]* %Mask2, i32 0, i32 %loadvalue21
  %loadvalue22 = load i32, i32* %array_member_addr
  %eq23 = icmp eq i32 %loadvalue22, 0
  br i1 %eq23, label %then24, label %ifcont25

then24:                                           ; preds = %then17
  store i1 true, i1* %skip
  br label %ifcont25

ifcont25:                                         ; preds = %then24, %then17
  %loadvalue26 = load i32, i32* %curr
  %array_member_addr27 = getelementptr [100 x i32], [100 x i32]* %Board1, i32 0, i32 %loadvalue26
  %loadvalue28 = load i32, i32* %array_member_addr27
  %ne = icmp ne i32 %loadvalue28, 0
  br i1 %ne, label %then29, label %ifcont30

then29:                                           ; preds = %ifcont25
  %loadvalue31 = load i32, i32* %curr
  %memberaddr = getelementptr [100 x i32], [100 x i32]* %Mask2, i32 0, i32 %loadvalue31
  store i32 0, i32* %memberaddr
  store i1 true, i1* %skip
  br label %ifcont30

ifcont30:                                         ; preds = %then29, %ifcont25
  %loadvalue32 = load i1, i1* %skip
  %eq33 = icmp eq i1 %loadvalue32, false
  br i1 %eq33, label %then34, label %ifcont35

then34:                                           ; preds = %ifcont30
  %loadvalue36 = load i32, i32* %curr
  %memberaddr37 = getelementptr [100 x i32], [100 x i32]* %Mask2, i32 0, i32 %loadvalue36
  store i32 0, i32* %memberaddr37
  %loadvalue38 = load [100 x i32], [100 x i32]* %Board1
  %loadvalue39 = load [100 x i32], [100 x i32]* %Mask2
  %loadvalue40 = load i32, i32* %i3
  %sub = sub i32 %loadvalue40, 1
  %loadvalue41 = load i32, i32* %j4
  %1 = call [100 x i32] @Main_BFS([100 x i32] %loadvalue38, [100 x i32] %loadvalue39, i32 %sub, i32 %loadvalue41)
  store [100 x i32] %1, [100 x i32]* %Mask2
  %loadvalue42 = load [100 x i32], [100 x i32]* %Board1
  %loadvalue43 = load [100 x i32], [100 x i32]* %Mask2
  %loadvalue44 = load i32, i32* %i3
  %loadvalue45 = load i32, i32* %j4
  %add = add i32 %loadvalue45, 1
  %2 = call [100 x i32] @Main_BFS([100 x i32] %loadvalue42, [100 x i32] %loadvalue43, i32 %loadvalue44, i32 %add)
  store [100 x i32] %2, [100 x i32]* %Mask2
  %loadvalue46 = load [100 x i32], [100 x i32]* %Board1
  %loadvalue47 = load [100 x i32], [100 x i32]* %Mask2
  %loadvalue48 = load i32, i32* %i3
  %loadvalue49 = load i32, i32* %j4
  %sub50 = sub i32 %loadvalue49, 1
  %3 = call [100 x i32] @Main_BFS([100 x i32] %loadvalue46, [100 x i32] %loadvalue47, i32 %loadvalue48, i32 %sub50)
  store [100 x i32] %3, [100 x i32]* %Mask2
  %loadvalue51 = load [100 x i32], [100 x i32]* %Board1
  %loadvalue52 = load [100 x i32], [100 x i32]* %Mask2
  %loadvalue53 = load i32, i32* %i3
  %loadvalue54 = load i32, i32* %j4
  %add55 = add i32 %loadvalue54, 1
  %4 = call [100 x i32] @Main_BFS([100 x i32] %loadvalue51, [100 x i32] %loadvalue52, i32 %loadvalue53, i32 %add55)
  store [100 x i32] %4, [100 x i32]* %Mask2
  %loadvalue56 = load [100 x i32], [100 x i32]* %Board1
  %loadvalue57 = load [100 x i32], [100 x i32]* %Mask2
  %loadvalue58 = load i32, i32* %i3
  %sub59 = sub i32 %loadvalue58, 1
  %loadvalue60 = load i32, i32* %j4
  %add61 = add i32 %loadvalue60, 1
  %5 = call [100 x i32] @Main_BFS([100 x i32] %loadvalue56, [100 x i32] %loadvalue57, i32 %sub59, i32 %add61)
  store [100 x i32] %5, [100 x i32]* %Mask2
  %loadvalue62 = load [100 x i32], [100 x i32]* %Board1
  %loadvalue63 = load [100 x i32], [100 x i32]* %Mask2
  %loadvalue64 = load i32, i32* %i3
  %sub65 = sub i32 %loadvalue64, 1
  %loadvalue66 = load i32, i32* %j4
  %sub67 = sub i32 %loadvalue66, 1
  %6 = call [100 x i32] @Main_BFS([100 x i32] %loadvalue62, [100 x i32] %loadvalue63, i32 %sub65, i32 %sub67)
  store [100 x i32] %6, [100 x i32]* %Mask2
  %loadvalue68 = load [100 x i32], [100 x i32]* %Board1
  %loadvalue69 = load [100 x i32], [100 x i32]* %Mask2
  %loadvalue70 = load i32, i32* %i3
  %add71 = add i32 %loadvalue70, 1
  %loadvalue72 = load i32, i32* %j4
  %add73 = add i32 %loadvalue72, 1
  %7 = call [100 x i32] @Main_BFS([100 x i32] %loadvalue68, [100 x i32] %loadvalue69, i32 %add71, i32 %add73)
  store [100 x i32] %7, [100 x i32]* %Mask2
  %loadvalue74 = load [100 x i32], [100 x i32]* %Board1
  %loadvalue75 = load [100 x i32], [100 x i32]* %Mask2
  %loadvalue76 = load i32, i32* %i3
  %add77 = add i32 %loadvalue76, 1
  %loadvalue78 = load i32, i32* %j4
  %sub79 = sub i32 %loadvalue78, 1
  %8 = call [100 x i32] @Main_BFS([100 x i32] %loadvalue74, [100 x i32] %loadvalue75, i32 %add77, i32 %sub79)
  store [100 x i32] %8, [100 x i32]* %Mask2
  br label %ifcont35

ifcont35:                                         ; preds = %then34, %ifcont30
  br label %ifcont18

ifcont18:                                         ; preds = %ifcont35, %ifcont15
  %loadvalue80 = load [100 x i32], [100 x i32]* %Mask2
  ret [100 x i32] %loadvalue80
}

define void @Main_main() {
entry:
  %Board = alloca [100 x i32]
  %Mask = alloca [100 x i32]
  %num_mines = alloca i32
  %init_i = alloca i32
  %init_pos = alloca i32
  %i = alloca i32
  %j = alloca i32
  %curr = alloca i32
  %num_surrounding_mins = alloca i32
  %u = alloca i32
  %d = alloca i32
  %l = alloca i32
  %r = alloca i32
  %ul = alloca i32
  %ur = alloca i32
  %dl = alloca i32
  %dr = alloca i32
  %game_status = alloca i32
  %board_val = alloca i32
  %X_char = alloca i8
  %E_char = alloca i8
  store i8 88, i8* %X_char
  store i8 10, i8* %E_char
  store i32 6, i32* %num_mines
  store i32 0, i32* %init_i
  br label %whilecond

whilecond:                                        ; preds = %while, %entry
  %loadvalue = load i32, i32* %init_i
  %lt = icmp slt i32 %loadvalue, 100
  br i1 %lt, label %while, label %next

while:                                            ; preds = %whilecond
  %loadvalue1 = load i32, i32* %init_i
  %memberaddr = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue1
  store i32 0, i32* %memberaddr
  %loadvalue2 = load i32, i32* %init_i
  %memberaddr3 = getelementptr [100 x i32], [100 x i32]* %Mask, i32 0, i32 %loadvalue2
  store i32 1, i32* %memberaddr3
  %loadvalue4 = load i32, i32* %init_i
  %add = add i32 %loadvalue4, 1
  store i32 %add, i32* %init_i
  br label %whilecond

next:                                             ; preds = %whilecond
  store i32 0, i32* %i
  br label %whilecond5

whilecond5:                                       ; preds = %while6, %next
  %loadvalue8 = load i32, i32* %i
  %loadvalue9 = load i32, i32* %num_mines
  %lt10 = icmp slt i32 %loadvalue8, %loadvalue9
  br i1 %lt10, label %while6, label %next7

while6:                                           ; preds = %whilecond5
  %0 = call i32 @Main_generateInitialPosition()
  store i32 %0, i32* %init_pos
  %loadvalue11 = load i32, i32* %init_pos
  %memberaddr12 = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue11
  store i32 -1, i32* %memberaddr12
  %loadvalue13 = load i32, i32* %i
  %add14 = add i32 %loadvalue13, 1
  store i32 %add14, i32* %i
  br label %whilecond5

next7:                                            ; preds = %whilecond5
  store i32 0, i32* %i
  store i32 0, i32* %j
  br label %whilecond15

whilecond15:                                      ; preds = %next22, %next7
  %loadvalue18 = load i32, i32* %i
  %lt19 = icmp slt i32 %loadvalue18, 10
  br i1 %lt19, label %while16, label %next17

while16:                                          ; preds = %whilecond15
  br label %whilecond20

whilecond20:                                      ; preds = %ifcont128, %while16
  %loadvalue23 = load i32, i32* %j
  %lt24 = icmp slt i32 %loadvalue23, 10
  br i1 %lt24, label %while21, label %next22

while21:                                          ; preds = %whilecond20
  %loadvalue25 = load i32, i32* %i
  %loadvalue26 = load i32, i32* %j
  %1 = call i32 @Main_computeBoardPosition(i32 %loadvalue25, i32 %loadvalue26)
  store i32 %1, i32* %curr
  %loadvalue27 = load i32, i32* %curr
  %array_member_addr = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue27
  %loadvalue28 = load i32, i32* %array_member_addr
  store i32 %loadvalue28, i32* %num_surrounding_mins
  %loadvalue29 = load i32, i32* %i
  %loadvalue30 = load i32, i32* %j
  %2 = call i32 @Main_getUpPosition(i32 %loadvalue29, i32 %loadvalue30)
  store i32 %2, i32* %u
  %loadvalue31 = load i32, i32* %u
  %ne = icmp ne i32 %loadvalue31, -1
  br i1 %ne, label %then, label %ifcont

then:                                             ; preds = %while21
  %loadvalue32 = load i32, i32* %u
  %array_member_addr33 = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue32
  %loadvalue34 = load i32, i32* %array_member_addr33
  %eq = icmp eq i32 %loadvalue34, -1
  br i1 %eq, label %then35, label %ifcont36

then35:                                           ; preds = %then
  %loadvalue37 = load i32, i32* %num_surrounding_mins
  %add38 = add i32 %loadvalue37, 1
  store i32 %add38, i32* %num_surrounding_mins
  br label %ifcont36

ifcont36:                                         ; preds = %then35, %then
  br label %ifcont

ifcont:                                           ; preds = %ifcont36, %while21
  %loadvalue39 = load i32, i32* %i
  %loadvalue40 = load i32, i32* %j
  %3 = call i32 @Main_getDownPosition(i32 %loadvalue39, i32 %loadvalue40)
  store i32 %3, i32* %d
  %loadvalue41 = load i32, i32* %d
  %ne42 = icmp ne i32 %loadvalue41, -1
  br i1 %ne42, label %then43, label %ifcont44

then43:                                           ; preds = %ifcont
  %loadvalue45 = load i32, i32* %d
  %array_member_addr46 = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue45
  %loadvalue47 = load i32, i32* %array_member_addr46
  %eq48 = icmp eq i32 %loadvalue47, -1
  br i1 %eq48, label %then49, label %ifcont50

then49:                                           ; preds = %then43
  %loadvalue51 = load i32, i32* %num_surrounding_mins
  %add52 = add i32 %loadvalue51, 1
  store i32 %add52, i32* %num_surrounding_mins
  br label %ifcont50

ifcont50:                                         ; preds = %then49, %then43
  br label %ifcont44

ifcont44:                                         ; preds = %ifcont50, %ifcont
  %loadvalue53 = load i32, i32* %i
  %loadvalue54 = load i32, i32* %j
  %4 = call i32 @Main_getLeftPosition(i32 %loadvalue53, i32 %loadvalue54)
  store i32 %4, i32* %l
  %loadvalue55 = load i32, i32* %l
  %ne56 = icmp ne i32 %loadvalue55, -1
  br i1 %ne56, label %then57, label %ifcont58

then57:                                           ; preds = %ifcont44
  %loadvalue59 = load i32, i32* %l
  %array_member_addr60 = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue59
  %loadvalue61 = load i32, i32* %array_member_addr60
  %eq62 = icmp eq i32 %loadvalue61, -1
  br i1 %eq62, label %then63, label %ifcont64

then63:                                           ; preds = %then57
  %loadvalue65 = load i32, i32* %num_surrounding_mins
  %add66 = add i32 %loadvalue65, 1
  store i32 %add66, i32* %num_surrounding_mins
  br label %ifcont64

ifcont64:                                         ; preds = %then63, %then57
  br label %ifcont58

ifcont58:                                         ; preds = %ifcont64, %ifcont44
  %loadvalue67 = load i32, i32* %i
  %loadvalue68 = load i32, i32* %j
  %5 = call i32 @Main_getRightPosition(i32 %loadvalue67, i32 %loadvalue68)
  store i32 %5, i32* %r
  %loadvalue69 = load i32, i32* %r
  %ne70 = icmp ne i32 %loadvalue69, -1
  br i1 %ne70, label %then71, label %ifcont72

then71:                                           ; preds = %ifcont58
  %loadvalue73 = load i32, i32* %r
  %array_member_addr74 = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue73
  %loadvalue75 = load i32, i32* %array_member_addr74
  %eq76 = icmp eq i32 %loadvalue75, -1
  br i1 %eq76, label %then77, label %ifcont78

then77:                                           ; preds = %then71
  %loadvalue79 = load i32, i32* %num_surrounding_mins
  %add80 = add i32 %loadvalue79, 1
  store i32 %add80, i32* %num_surrounding_mins
  br label %ifcont78

ifcont78:                                         ; preds = %then77, %then71
  br label %ifcont72

ifcont72:                                         ; preds = %ifcont78, %ifcont58
  %loadvalue81 = load i32, i32* %i
  %loadvalue82 = load i32, i32* %j
  %6 = call i32 @Main_getUpLeftPosition(i32 %loadvalue81, i32 %loadvalue82)
  store i32 %6, i32* %ul
  %loadvalue83 = load i32, i32* %ul
  %ne84 = icmp ne i32 %loadvalue83, -1
  br i1 %ne84, label %then85, label %ifcont86

then85:                                           ; preds = %ifcont72
  %loadvalue87 = load i32, i32* %ul
  %array_member_addr88 = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue87
  %loadvalue89 = load i32, i32* %array_member_addr88
  %eq90 = icmp eq i32 %loadvalue89, -1
  br i1 %eq90, label %then91, label %ifcont92

then91:                                           ; preds = %then85
  %loadvalue93 = load i32, i32* %num_surrounding_mins
  %add94 = add i32 %loadvalue93, 1
  store i32 %add94, i32* %num_surrounding_mins
  br label %ifcont92

ifcont92:                                         ; preds = %then91, %then85
  br label %ifcont86

ifcont86:                                         ; preds = %ifcont92, %ifcont72
  %loadvalue95 = load i32, i32* %i
  %loadvalue96 = load i32, i32* %j
  %7 = call i32 @Main_getUpRightPosition(i32 %loadvalue95, i32 %loadvalue96)
  store i32 %7, i32* %ur
  %loadvalue97 = load i32, i32* %ur
  %ne98 = icmp ne i32 %loadvalue97, -1
  br i1 %ne98, label %then99, label %ifcont100

then99:                                           ; preds = %ifcont86
  %loadvalue101 = load i32, i32* %ur
  %array_member_addr102 = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue101
  %loadvalue103 = load i32, i32* %array_member_addr102
  %eq104 = icmp eq i32 %loadvalue103, -1
  br i1 %eq104, label %then105, label %ifcont106

then105:                                          ; preds = %then99
  %loadvalue107 = load i32, i32* %num_surrounding_mins
  %add108 = add i32 %loadvalue107, 1
  store i32 %add108, i32* %num_surrounding_mins
  br label %ifcont106

ifcont106:                                        ; preds = %then105, %then99
  br label %ifcont100

ifcont100:                                        ; preds = %ifcont106, %ifcont86
  %loadvalue109 = load i32, i32* %i
  %loadvalue110 = load i32, i32* %j
  %8 = call i32 @Main_getDownLeftPosition(i32 %loadvalue109, i32 %loadvalue110)
  store i32 %8, i32* %dl
  %loadvalue111 = load i32, i32* %dl
  %ne112 = icmp ne i32 %loadvalue111, -1
  br i1 %ne112, label %then113, label %ifcont114

then113:                                          ; preds = %ifcont100
  %loadvalue115 = load i32, i32* %dl
  %array_member_addr116 = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue115
  %loadvalue117 = load i32, i32* %array_member_addr116
  %eq118 = icmp eq i32 %loadvalue117, -1
  br i1 %eq118, label %then119, label %ifcont120

then119:                                          ; preds = %then113
  %loadvalue121 = load i32, i32* %num_surrounding_mins
  %add122 = add i32 %loadvalue121, 1
  store i32 %add122, i32* %num_surrounding_mins
  br label %ifcont120

ifcont120:                                        ; preds = %then119, %then113
  br label %ifcont114

ifcont114:                                        ; preds = %ifcont120, %ifcont100
  %loadvalue123 = load i32, i32* %i
  %loadvalue124 = load i32, i32* %j
  %9 = call i32 @Main_getDownRightPosition(i32 %loadvalue123, i32 %loadvalue124)
  store i32 %9, i32* %dr
  %loadvalue125 = load i32, i32* %dr
  %ne126 = icmp ne i32 %loadvalue125, -1
  br i1 %ne126, label %then127, label %ifcont128

then127:                                          ; preds = %ifcont114
  %loadvalue129 = load i32, i32* %dr
  %array_member_addr130 = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue129
  %loadvalue131 = load i32, i32* %array_member_addr130
  %eq132 = icmp eq i32 %loadvalue131, -1
  br i1 %eq132, label %then133, label %ifcont134

then133:                                          ; preds = %then127
  %loadvalue135 = load i32, i32* %num_surrounding_mins
  %add136 = add i32 %loadvalue135, 1
  store i32 %add136, i32* %num_surrounding_mins
  br label %ifcont134

ifcont134:                                        ; preds = %then133, %then127
  br label %ifcont128

ifcont128:                                        ; preds = %ifcont134, %ifcont114
  %loadvalue137 = load i32, i32* %num_surrounding_mins
  %loadvalue138 = load i32, i32* %curr
  %memberaddr139 = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue138
  store i32 %loadvalue137, i32* %memberaddr139
  %loadvalue140 = load i32, i32* %j
  %add141 = add i32 %loadvalue140, 1
  store i32 %add141, i32* %j
  br label %whilecond20

next22:                                           ; preds = %whilecond20
  store i32 0, i32* %j
  %loadvalue142 = load i32, i32* %i
  %add143 = add i32 %loadvalue142, 1
  store i32 %add143, i32* %i
  br label %whilecond15

next17:                                           ; preds = %whilecond15
  store i32 0, i32* %game_status
  br label %whilecond144

whilecond144:                                     ; preds = %ifcont199, %next17
  %loadvalue147 = load i32, i32* %game_status
  %eq148 = icmp eq i32 %loadvalue147, 0
  br i1 %eq148, label %while145, label %next146

while145:                                         ; preds = %whilecond144
  store i32 0, i32* %i
  store i32 0, i32* %j
  br label %whilecond149

whilecond149:                                     ; preds = %next156, %while145
  %loadvalue152 = load i32, i32* %i
  %lt153 = icmp slt i32 %loadvalue152, 10
  br i1 %lt153, label %while150, label %next151

while150:                                         ; preds = %whilecond149
  br label %whilecond154

whilecond154:                                     ; preds = %ifcont169, %while150
  %loadvalue157 = load i32, i32* %j
  %lt158 = icmp slt i32 %loadvalue157, 10
  br i1 %lt158, label %while155, label %next156

while155:                                         ; preds = %whilecond154
  %loadvalue159 = load i32, i32* %i
  %loadvalue160 = load i32, i32* %j
  %10 = call i32 @Main_computeBoardPosition(i32 %loadvalue159, i32 %loadvalue160)
  store i32 %10, i32* %curr
  %loadvalue161 = load i32, i32* %curr
  %array_member_addr162 = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue161
  %loadvalue163 = load i32, i32* %array_member_addr162
  store i32 %loadvalue163, i32* %board_val
  %loadvalue164 = load i32, i32* %curr
  %array_member_addr165 = getelementptr [100 x i32], [100 x i32]* %Mask, i32 0, i32 %loadvalue164
  %loadvalue166 = load i32, i32* %array_member_addr165
  %eq167 = icmp eq i32 %loadvalue166, 1
  br i1 %eq167, label %then168, label %else

then168:                                          ; preds = %while155
  %load_for_cast = load i8, i8* %X_char
  %basic_type_cast = sext i8 %load_for_cast to i32
  %11 = call i32 @putchar(i32 %basic_type_cast)
  br label %ifcont169

ifcont169:                                        ; preds = %else, %then168
  %loadvalue173 = load i32, i32* %j
  %add174 = add i32 %loadvalue173, 1
  store i32 %add174, i32* %j
  br label %whilecond154

else:                                             ; preds = %while155
  %loadvalue170 = load i32, i32* %board_val
  %add171 = add i32 %loadvalue170, 48
  store i32 %add171, i32* %board_val
  %loadvalue172 = load i32, i32* %board_val
  %12 = call i32 @putchar(i32 %loadvalue172)
  br label %ifcont169

next156:                                          ; preds = %whilecond154
  %load_for_cast175 = load i8, i8* %E_char
  %basic_type_cast176 = sext i8 %load_for_cast175 to i32
  %13 = call i32 @putchar(i32 %basic_type_cast176)
  store i32 0, i32* %j
  %loadvalue177 = load i32, i32* %i
  %add178 = add i32 %loadvalue177, 1
  store i32 %add178, i32* %i
  br label %whilecond149

next151:                                          ; preds = %whilecond149
  store i32 0, i32* %i
  store i32 0, i32* %j
  %14 = call i32 @puts(i8* getelementptr inbounds ([19 x i8], [19 x i8]* @0, i32 0, i32 0))
  %15 = call i32 @getchar()
  %sub = sub i32 %15, 1
  store i32 %sub, i32* %i
  %16 = call i32 @getchar()
  %17 = call i32 @puts(i8* getelementptr inbounds ([19 x i8], [19 x i8]* @1, i32 0, i32 0))
  %18 = call i32 @getchar()
  %sub179 = sub i32 %18, 1
  store i32 %sub179, i32* %j
  %19 = call i32 @getchar()
  %loadvalue180 = load i32, i32* %i
  %sub181 = sub i32 %loadvalue180, 48
  store i32 %sub181, i32* %i
  %loadvalue182 = load i32, i32* %j
  %sub183 = sub i32 %loadvalue182, 48
  store i32 %sub183, i32* %j
  %loadvalue184 = load i32, i32* %i
  %loadvalue185 = load i32, i32* %j
  %20 = call i32 @Main_computeBoardPosition(i32 %loadvalue184, i32 %loadvalue185)
  store i32 %20, i32* %curr
  %loadvalue186 = load [100 x i32], [100 x i32]* %Board
  %loadvalue187 = load [100 x i32], [100 x i32]* %Mask
  %loadvalue188 = load i32, i32* %i
  %loadvalue189 = load i32, i32* %j
  %21 = call [100 x i32] @Main_BFS([100 x i32] %loadvalue186, [100 x i32] %loadvalue187, i32 %loadvalue188, i32 %loadvalue189)
  store [100 x i32] %21, [100 x i32]* %Mask
  %loadvalue190 = load i32, i32* %curr
  %array_member_addr191 = getelementptr [100 x i32], [100 x i32]* %Board, i32 0, i32 %loadvalue190
  %loadvalue192 = load i32, i32* %array_member_addr191
  %eq193 = icmp eq i32 %loadvalue192, -1
  br i1 %eq193, label %then194, label %ifcont195

then194:                                          ; preds = %next151
  store i32 2, i32* %game_status
  br label %ifcont195

ifcont195:                                        ; preds = %then194, %next151
  %loadvalue196 = load [100 x i32], [100 x i32]* %Mask
  %loadvalue197 = load i32, i32* %num_mines
  %22 = call i1 @Main_isWin([100 x i32] %loadvalue196, i32 %loadvalue197)
  br i1 %22, label %then198, label %ifcont199

then198:                                          ; preds = %ifcont195
  store i32 1, i32* %game_status
  br label %ifcont199

ifcont199:                                        ; preds = %then198, %ifcont195
  br label %whilecond144

next146:                                          ; preds = %whilecond144
  %loadvalue200 = load i32, i32* %game_status
  %eq201 = icmp eq i32 %loadvalue200, 1
  br i1 %eq201, label %then202, label %else204

then202:                                          ; preds = %next146
  %23 = call i32 @puts(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @2, i32 0, i32 0))
  br label %ifcont203

ifcont203:                                        ; preds = %else204, %then202
  ret void

else204:                                          ; preds = %next146
  %24 = call i32 @puts(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @3, i32 0, i32 0))
  br label %ifcont203
}
