cmd_net/xia/xia.ko := ld -r -m elf_x86_64 -T ./scripts/module-common.lds --build-id  -o net/xia/xia.ko net/xia/xia.o net/xia/xia.mod.o
