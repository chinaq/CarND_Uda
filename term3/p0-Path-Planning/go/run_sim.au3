;~ #include ".\close_sim.au3"

;~ close win
Local $winTitle = 'self_driving_car_nanodegree_program'
WinClose($winTitle)

;~ run win
Run(@ScriptDir & "\..\..\term3_sim_windows\term3_sim.exe")
WinWaitActive($winTitle, "", 3)

;~ click select
If WinActivate($winTitle) THEN
    ;~ ConsoleWrite("sim activated")
    Send("{ENTER}")
    Sleep(8000)
    Local $pos = WinGetPos($winTitle)
    Local $x =  ($pos[0] + $pos[2]*0.5)
    Local $y =  ($pos[1] + $pos[3]*0.8)
    MouseClick('left', $x, $y)
Else
    ;~ ConsoleWrite("sim error")
    Exit(1)
EndIf
