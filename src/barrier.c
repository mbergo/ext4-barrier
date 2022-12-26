static int ext4_remount(struct super_block *sb, int *flags, char *data)
{
    struct ext4_sb_info *sbi = EXT4_SB(sb);
    struct ext4_super_block *es = sbi->s_es;
    unsigned long old_sb_flags = sb->s_flags;
    int enable_quota = 0;
    int err = 0;
    unsigned int journal_ioprio = DEFAULT_JOURNAL_IOPRIO;
    char *orig_data = kstrdup(data, GFP_KERNEL);
    char *journal_devname = NULL;
    int old_barrier = test_opt(sb, BARRIER);
    int old_errors = test_opt(sb, ERRORS_RO);
    int old_commit = 0;
    char *old_jcharset = NULL;
    unsigned int old_journal_flags;

    /* Store the original options */
    sbi->s_mount_opt = sbi->s_mount_opt & ~EXT4_MOUNT_BARRIER;
    if (old_barrier)
        sbi->s_mount_opt |= EXT4_MOUNT_BARRIER;
    if (old_errors)
        sbi->s_mount_opt |= EXT4_MOUNT_ERRORS_RO;
    ext4_clear_journal_err(sb, es);

    /*
     * Allow the "barrier" mount option to be passed as a remount
     * option.
     */
    if (!parse_options(data, sb, NULL, &journal_devname,
               &journal_ioprio, NULL, 0))
        goto restore_opts;

    if (test_opt(sb, DATA_FLAGS) == EXT4_MOUNT_JOURNAL_DATA) {
        ext4_msg(sb, KERN_ERR, "remounting with journal data mode "
             "not supported");
        err = -EINVAL;
        goto restore_opts;
    }

    sb->s_flags = (sb->s_flags & ~MS_POSIXACL) |
        (test_opt(sb, POSIX_ACL) ? MS_POSIXACL : 0);

    if (test_opt(sb, BARRIER))
        *flags |= MS_BARRIER;
    else
        *flags &= ~MS_BARRIER;


