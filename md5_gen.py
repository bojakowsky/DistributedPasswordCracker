import md5
import random 
def h(f, str, last=False):
	m = md5.new()
	m.update(str)
	res = m.hexdigest()
	if not last:
		res = res + "\n"
	f.write(res)

f = open('passwords', 'w')

h(f, "zak")
h(f, "abc")
h(f, "bed")
h(f, "KxC")
h(f, "!!1")
h(f, "1Zd")
h(f, "ab")
h(f, "!k")
h(f, "JH")
h(f, "15")
h(f, "kot")
h(f, "lot")
h(f, "xyz")

#numbers permutation
#for i in range(100,900):
#	h(f, str(random.randrange(0, 1000)))	

h(f, "end", True)

f.close()

#examples 
#fbade9e36a3f36d3d676c1b808451dd7 z
#0cc175b9c0f1b6a831c399e269772661 a
#92eb5ffee6ae2fec3ad71c777531578f b
#a5f3c6a11b03839d46af9fb43c97c188 K
#21c2e59531c8710156d34a3c30ac81d5 Z
#9033e0e305f247c0c3c80d0c7848c8b3 !
#cfcd208495d565ef66e7dff9f98764da 0
#187ef4436122d1cc2f40dc2b92f0eba0 ab
#cbc04dde0603cf07934f55fd6354c002 !k
#deaa993adb5bb29de13f501a8e623be4 JH
#9bf31c7ff062936a96d3c8bd1f8f2ff3 15
#a986d9ee785f7b5fdd68bb5b86ee70e0 kot
#a51c008b213c7a0462e49895df164b50 qwe1
#d8578edf8458ce06fbc5bb76a58c5ca4 qwerty1
#76a2173be6393254e72ffa4d6df1030a passwd
