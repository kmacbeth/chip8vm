class Assembler:

    def __init__(self, output_file):
        self._output_file = output_file

    def load_register_with_value(self, reg_index, reg_value):
        opcode = 0x6000
        opcode |= ((reg_index & 0xF) << 8)
        opcode |= (reg_value & 0xFF)
        self._write_opcode(opcode)

    def load_font(self, reg_index):
        opcode = 0xF029
        opcode |= ((reg_index & 0xF) << 8)
        self._write_opcode(opcode)

    def draw(self, reg_index1, reg_index2, size):
        opcode = 0xD000
        opcode |= ((reg_index1 & 0xF) << 8)
        opcode |= ((reg_index2 & 0xF) << 4)
        opcode |= size & 0xF
        self._write_opcode(opcode)

    def jump(self, address):
        opcode = 0xB000
        opcode |= (address & 0xFFF)
        self._write_opcode(opcode)

    def _write_opcode(self, opcode):
        self._output_file.write(opcode.to_bytes(2, byteorder='little'))


def assemble():
    with open("output.bin", "wb") as output_file:
        asm = Assembler(output_file)
        x, y = 2, 2
        offset_x, offset_y = 8, 6

        for c in range(16):
            asm.load_register_with_value(0, c)
            asm.load_font(0)
            asm.load_register_with_value(1, x)
            asm.load_register_with_value(2, y)
            asm.draw(1, 2, 5)

            x += offset_x
            if x > (4 * offset_x):
                x = 2
                y += 1

        asm.load_register_with_value(0, 0)
        asm.jump(0x2A3)


if __name__ == '__main__':
    assemble()
