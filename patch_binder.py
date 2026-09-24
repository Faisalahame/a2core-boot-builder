import glob
hits=0
for f in glob.glob('drivers/**/binder.c', recursive=True):
    t=open(f).read()
    a='void __user *ubub = (void __user *)arg;'
    a='void __user *ubuf = (void __user *)arg;'
    b='case BINDER_VERSION:'
    if a in t and b in t and 'IOCTL pid=' not in t:
        t=t.replace(a, a+'\n\tpr_info("binder: IOCTL pid=%d comm=%s cmd=%08x\\n", current->pid, current->comm, cmd);',1)
        t=t.replace(b, b+'\n\t\tpr_info("binder: BINDER_VERSION pid=%d val=%d\\n", current->pid, BINDER_CURRENT_PROTOCOL_VERSION);',1)
        open(f,'w').write(t); hits+=1; print('patched', f)
print('files patched:', hits)
