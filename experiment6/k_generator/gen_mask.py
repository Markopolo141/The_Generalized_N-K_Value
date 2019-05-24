import click

@click.command(short_help='generate c++ sourcecode file for mask')
@click.argument('n', metavar='<LONGS>', type=click.INT)
@click.argument('f', metavar='<FILE>', default="mask.cpp", type=click.File('w'))
def gen_mask(n, f):
	"""This script generates a C++ source code file named <FILE> (default 'mask.cpp'). for class and associated methods for handling binary masks of size <LONGS> unsigned longs big."""
	assert n>0
	warning_string = 'if (({{0}}>=sizeof(unsigned long)*8*{}) || ({{0}}<0)) {{{{\n\t\t\t printf("ERROR: mask bit overflow\\n");\n\t\t}}}}'.format(n)
	f.write("//AUTO GENERATED CODE FILE, by gen_mask.py\n\n")
	f.write("inline int count_the_bits(unsigned long A) {\n\tunsigned long B;\n\tB = A;\n\tint i=0;\n\twhile (B!=0) {\n\t\ti += B&1;\n\t\tB >>= 1;\n\t}\n\treturn i;\n}\n\n");
	f.write("class Mask {\n\tpublic:\n")
	for i in range(n):
		f.write("\tunsigned long A{};\n".format(i))
	f.write("\tstatic const unsigned long length = sizeof(unsigned long)*8*{};\n\n".format(n))
	f.write("\tvoid print() {\n")
	f.write('\t\tfor (unsigned long i=0; i<this->length; i++) { printf("%i",this->get_bit(i)); } printf("\\n");\n\t}\n')
	f.write('\tinline bool operator==(const Mask& m) {\n')
	f.write('\t\tif({})\n\t\t\treturn true;\n\t\treturn false;\n\t}}\n'.format(' && '.join(["(m.A{} == this->A{})".format(i,i) for i in range(n)])))
	f.write('\tinline Mask operator&(const Mask& m) {{\n\t\tMask m2;\n\t\t{}\n\t\treturn m2;\n\t}}\n'.format("\n\t\t".join(["m2.A{} = (this->A{})&(m.A{});".format(i,i,i) for i in range(n)])))
	f.write('\tinline Mask operator~() {{\n\t\tMask m2;\n\t\t{}\n\t\treturn m2;\n\t}}\n'.format("\n\t\t".join(["m2.A{} = ~(this->A{});".format(i,i) for i in range(n)])))
	f.write('\tinline Mask operator^(const Mask& m) {{\n\t\tMask m2;\n\t\t{}\n\t\treturn m2;\n\t}}\n'.format("\n\t\t".join(["m2.A{} = (this->A{})^(m.A{});".format(i,i,i) for i in range(n)])))
	f.write('\tinline void func_xor(Mask* m) {{\n\t\t{}\n\t\treturn;\n\t}}\n'.format("\n\t\t".join(["this->A{} = (this->A{})^(m->A{});".format(i,i,i) for i in range(n)])))
	f.write('\tinline Mask operator|(const Mask& m) {{\n\t\tMask m2;\n\t\t{}\n\t\treturn m2;\n\t}}\n'.format("\n\t\t".join(["m2.A{} = (this->A{})|(m.A{});".format(i,i,i) for i in range(n)])))
	f.write('\tinline void set_zero() {{\n\t\t{}\n\t}}\n'.format("\n\t\t".join(["this->A{} = 0;".format(i) for i in range(n)])))
	f.write('\tinline void operator=(Mask* m) {{\n\t\t{}\n\t}}\n'.format("\n\t\t".join(["this->A{} = m->A{};".format(i,i) for i in range(n)])))
	f.write('\tinline void set(Mask* m) {{\n\t\t{}\n\t}}\n'.format("\n\t\t".join(["this->A{} = m->A{};".format(i,i) for i in range(n)])))
	f.write('\tinline void set(Mask m) {{\n\t\t{}\n\t}}\n'.format("\n\t\t".join(["this->A{} = m.A{};".format(i,i) for i in range(n)])))
	f.write('\tinline void flip_bit(unsigned int bit) {{\n\t\t{}\n\t\t{}\n\t}}\n'.format(warning_string.format("bit"),'\n\t\t'.join(["if (bit>=sizeof(unsigned long)*8*{}) {{\n\t\t\tthis->A{} ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*{}); return;\n\t\t}}".format(i,i,i) for i in range(n-1,-1,-1)])))
	f.write('\tint count_bits() {{\n\t\tint a=0;\n\t\t{}\n\t\treturn a;\n\t}}\n'.format('\n\t\t'.join(['a += count_the_bits(this->A{});'.format(i) for i in range(n)])))
	f.write('\tinline unsigned int get_bit(unsigned int bit) {{\n\t\t{}\n\t\t{}\n\t}}\n'.format(warning_string.format('bit'),'\n\t\t'.join(['if (bit>=sizeof(unsigned long)*8*{}) {{\n\t\t\treturn (A{} & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*{}))!=0;\n\t\t}}'.format(i,i,i) for i in range(n-1,-1,-1)])))
	f.write('\tinline void set_bit(unsigned int bit, unsigned int value) {{\n\t\t{}\n\t\tif (value==1) {{\n\t\t\t{}\n\t\t}} else {{\n\t\t\t{}\n\t\t}}\n\t}}\n'.format(warning_string.format('bit'),"\n\t\t\t".join(["if (bit>=sizeof(unsigned long)*8*{}) {{A{} |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*{});return;}}".format(i,i,i) for i in range(n-1,-1,-1)]),"\n\t\t\t".join(["if (bit>=sizeof(unsigned long)*8*{}) {{A{} &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*{}));return;}}".format(i,i,i) for i in range(n-1,-1,-1)])))
	f.write('\tinline void remove_bit(unsigned int bit) {{\n\t\t{}\n\t\tunsigned long t1;\n\t\tunsigned long t2;\n\t\t{}\n\t}}\n'.format(warning_string.format("bit"),"\n\t\t".join(["if (bit>=sizeof(unsigned long)*8*{}) {{\n\t\t\tbit -= sizeof(unsigned long)*8*{};\n\t\t\tt1 = (A{}>>bit)<<bit;\n\t\t\tt2 = (A{}>>(bit+1))<<(bit);\n\t\t\tA{} = (A{}^t1)|t2;\n\t\t\t{}\n\t\t\treturn;\n\t\t}}".format(i,i,i,i,i,i,"\n\t\t\t".join(["A{} |= ((A{}&1)<<(sizeof(unsigned long)*8-1)); A{} >>= 1;".format(ii,ii+1,ii+1) for ii in range(i,n-1)])) for i in range(n-1,-1,-1)])))
	f.write('\tinline bool non_zero() {{\n\t\tif (({})==0)\n\t\t\treturn false;\n\t\treturn true;\n\t}}\n'.format("|".join(["(this->A{})".format(i) for i in range(n)])))
	f.write('\tinline void set_ones(unsigned int ones) {{\n\t\tthis->set_zero();\n\t\tif ((ones>sizeof(unsigned long)*8*{})||(ones<0)) {{\n\t\t\tprintf("ERROR: mask bit overflow\\n"); return;\n\t\t}}\n\t\t'.format(n))
	f.write('if (ones==sizeof(unsigned long)*8*{}) {{\n\t\t\t{}\n\t\t\treturn;\n\t\t}}\n\t\t'.format(n,"\n\t\t\t".join(["A{} = ~((unsigned long)0);".format(i) for i in range(n-1,-1,-1)]),n))
	f.write("\n\t\t".join(["if (ones>=sizeof(unsigned long)*8*{}) {{\n\t\t\t{}\n\t\t\t{}\n\t\t\treturn;\n\t\t}}".format(i,"A{} = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8*{}))-1);".format(i,i),"\n\t\t\t".join(["A{} = ~((unsigned long)0);".format(ii) for ii in range(i-1,-1,-1)])) for i in range(n-1,-1,-1)]))
	f.write("\n\t}\n")
	f.write("\tvoid set_long(int i, unsigned long l) {{\n\t\t{}\n\t\t{}\n\t}}\n".format(warning_string.format("i*(sizeof(unsigned long)*8)"),"\n\t\t".join(["if (i=={}) {{ A{}=l; }}".format(i,i) for i in range(n)])))
	f.write("};\n")
	f.close()

if __name__ == '__main__':
    gen_mask()

