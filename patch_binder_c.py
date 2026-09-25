import glob
hits = 0
for f in glob.glob('**/binder.c', recursive=True):
    with open(f, 'r') as file:
        content = file.read()
    original = content
    # Hardcode the return value to 7 directly in the ioctl handler
    content = content.replace('put_user(BINDER_CURRENT_PROTOCOL_VERSION', 'put_user(7')
    if content != original:
        with open(f, 'w') as file:
            file.write(content)
        hits += 1
        print(f'Patched {f} to hardcode Protocol 7')
print(f'Total files patched: {hits}')
