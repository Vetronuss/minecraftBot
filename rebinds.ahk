#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
; #Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.
#SingleInstance Force

;this script converts the default c++ windows virtual keys to ahk keys that can be read system wide



F24::send {ENTER}

F23::send {/}

F22::send {@}

F20::send {_}

F19::send {text}==
F18::send {escape}
F17::send {e}
F16::send {1}

F21::
Send {Shift down}{LButton down}
Send {Shift up}{LButton up}
Send {LButton up}
return

F14::
Send {LCtrl down}{q down}
Send {LCtrl up}{q up}
Send {q up}
return


F15::
send {enter}
sleep, 200
send {text}@goto ~ ~2 ~
sleep, 100
send {enter}
sleep, 4000
send {enter}
sleep, 200
send {text}@goto ~ ~-2 ~
sleep, 100
send {enter}
sleep, 4000
send {1}
sleep, 500
send {enter}
sleep, 100
send {text}.toggle LookLock
sleep, 100
send {enter}

sleep, 100
send {SPACE down}
sleep, 300
send {lbutton}
send {SPACE up}
sleep, 50
send {enter}
sleep, 100
send {text}.toggle LookLock
sleep, 100
send {enter}

return
