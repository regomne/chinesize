class ByteFile:
    "Operate String as a File."
    pData: int = 0

    def __init__(self, Src: bytes):
        self.pData = 0
        self.b = bytearray(Src)

    def __len__(self) -> int:
        return len(self.b)

    def __getitem__(self, i: int) -> int:
        return self.b[i]

    def __setitem__(self, i: int, y: int):
        self.b[i] = y

    def get_slice(self, i: int, j: int) -> bytearray:
        return self.b[i:j]

    def get_slice2(self, i: int) -> bytearray:
        return self.b[i:]

    def read(self, Bytes: int = -1) -> bytearray:
        if Bytes < -1:
            return bytearray(b"")
        elif Bytes == -1:
            tData = self.b[self.pData :]
            self.pData = len(self.b)
        else:
            tData = self.b[self.pData : self.pData + Bytes]
            self.pData += Bytes
        return tData

    def tell(self) -> int:
        return self.pData

    def seek(self, Offset: int, mode: int = 0):
        if mode == 1:
            Offset += self.pData
        elif mode == 2:
            Offset = len(self.b) - Offset
        if Offset < 0:
            self.pData += Offset
        elif Offset < len(self.b):
            self.pData = Offset
        else:
            self.pData = len(self.b)

    def readstr(self) -> bytearray:
        p = self.b.find(b"\0", self.pData)
        if p == -1:
            p0 = self.pData
            self.pData = len(self.b)
            return self.b[p0:]
        p0 = self.pData
        self.pData = p + 1
        return self.b[p0:p]

    def readu32(self) -> int:
        tData = self.b[self.pData : self.pData + 4]
        self.pData += 4
        if len(tData) < 4:
            return 0
        u32 = (tData[3] << 24) + (tData[2] << 16) + (tData[1] << 8) + (tData[0])
        return u32

    def readu16(self) -> int:
        tData = self.b[self.pData : self.pData + 2]
        self.pData += 2
        if len(tData) < 2:
            return 0
        return (tData[1] << 8) + (tData[0])

    def readu8(self) -> int:
        if self.pData >= len(self.b):
            return 0
        self.pData += 1
        return self.b[self.pData - 1]

    def readstrp(self, pos: int) -> bytearray:
        p = self.b.find(b"\0", pos)
        if p == -1:
            return self.b[pos:]
        return self.b[pos:p]

    def readu32p(self, pos: int) -> int:
        if pos + 3 >= len(self.b):
            return 0
        return (
            (self.b[pos + 3] << 24)
            + (self.b[pos + 2] << 16)
            + (self.b[pos + 1] << 8)
            + self.b[pos]
        )

    def readu16p(self, pos: int) -> int:
        if pos + 1 >= len(self.b):
            return 0
        return (self.b[pos + 1] << 8) + self.b[pos]

    def readu8p(self, pos: int) -> int:
        if pos >= len(self.b):
            return 0
        return self.b[pos]
