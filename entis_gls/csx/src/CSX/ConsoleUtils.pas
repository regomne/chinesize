unit ConsoleUtils;

interface

uses Windows;

type
  TArray = array of String;

{ Not a console-related function though... }
function Explode(Delimiter, Value: String; Count: Integer = 0): TArray;


{ Outputs a message, waits for a user input and terminates }
procedure Error(Msg: String);

{ Returns current console pointer position }
function GetXY(IncY: SmallInt): TCoord;

{ Resets a progress bar (next call to UpdateProgress will start a new one) }
procedure ResetProgress;

{ Outputs a progress bar }
procedure UpdateProgress(Current, Max: DWord; Extra: String = '');

function GetConsole: THandle;

implementation

uses SysUtils;

var
  Console: THandle;
  LastPro: Word;

function Explode;
var
  S, Str: string;
  I, P: Integer;
begin
  S := Value;
  I := 0;
  SetLength(Result, 0);

  while Length(S) > 0 do
  begin
    Inc(I);
    SetLength(Result, I);
    P := Pos(Delimiter, S);

    if (P > 0) and ((I < Count) or (Count = 0)) then
     begin
      Str := Copy(S, 0, P-1);
      if Str <> '' then
        result[I - 1] := Trim(Str)
        else
          Dec(I);
      S := Copy(S, P + Length(Delimiter), Length(S))
    end
      else
      begin
        Result[I - 1] := Trim(S);
        S := ''
      end
  end
end;


procedure Error;
begin
  WriteLn('Error: ' + Msg);
  ReadLn;
  Halt(1)
end;

function GetXY;
var
  Info: TConsoleScreenBufferInfo;
begin
  FillMemory(@Result, SizeOf(Result), 0);
  if GetConsoleScreenBufferInfo(Console, Info) then
    with Info.dwCursorPosition do
    begin
      Result.X := X;
      Result.Y := Y + IncY
    end
end;

procedure ResetProgress;
begin
  LastPro := $FFFF
end;

procedure UpdateProgress;
var
  Pro: Word;
  Str: String;
begin
  Pro := Round(100 * (Current / Max));
  if Pro <> LastPro then
  begin
    if LastPro <> $FFFF then
      SetConsoleCursorPosition(Console, GetXY(-1));
    Str := '  [ ' + IntToStr(Pro) + '% ]... ' + Extra;
    WriteLn(Str, StringOfChar(' ', 79 - Length(Str)));
    LastPro := Pro
  end;
end;

function GetConsole: THandle;
begin
  Result := Console;
end;

initialization
  Console := GetStdHandle(STD_OUTPUT_HANDLE);
  ResetProgress;

end.

