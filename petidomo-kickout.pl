:
eval 'exec perl -S $0 ${1+"$@"}'
    if $running_under_some_shell;
##
##  petidomo-kickout -- Petidomo kickout tool
##  Copyright (c) 2000 Ralf S. Engelschall <rse@engelschall.com>
##

my $pattern = $ARGV[0] || die "Usage: petidomo-kickout <address-pattern>";

#   locate the file with the approval passwords
my $config = "$ENV{HOME}/.petidomo";

#   locate the Sendmail program
my $sendmail = "sendmail";
foreach my $dir (split(/:/, "$ENV{PATH}:/bin:/sbin:/usr/bin:/usr/sbin:/lib:/usr/lib")) {
    if (-x "$dir/sendmail") {
        $sendmail = "$dir/sendmail";
        last;
    }
}

#   read config
my $list = {};
open(FP, "<$config") || die "No ~/.petidomo file found";
while (<FP>) {
    next if (m|^\s*#.*| or m|^\s*$|);
    if (m|^\s*(\S+)\s+(\S+)|) {
        my ($l, $pw) = ($1, $2);
        $l =~ s|@[^@]+$||;
        $list->{$l} = {};
        $list->{$l}->{PASSWORD} = $pw;
        $list->{$l}->{MEMBERS} = [];
        next if ($l eq 'petidomo');
        open(DUMP, "./petidomo --masterconf=../test/petidomo.conf --mode=dump $l |") || die "fuck";
        while (<DUMP>) {
            s|\n$||s;
            push(@{$list->{$l}->{MEMBERS}}, $_);
        }
        close(DUMP);
    }
}
close(FP);

#   iterate over all mailing lists
foreach my $l (keys(%{$list})) {
    foreach my $m (@{$list->{$l}->{MEMBERS}}) {
        if ($m =~ m|$pattern|) {
            print "petidomo-kickout: $l: <$m> Kickout? [Y/n] ";
            my $yn = <STDIN>;
            $yn =~ s|\n$||s;
            $yn = "y" if ($yn eq '');
            $yn = lc($yn);
            if ($yn eq 'y') {
                open(SM, "|$sendmail petidomo") || die "cannot spawn $sendmail";
                print SM "To: petidomo\n" .
                         "\n" .
                         "password ".$list->{$l}->{PASSWORD}."\n" .
                         "unsubscribe $l $m\n";
                         "password ".$list->{petidomo}->{PASSWORD}."\n" .
                         "subscribe bounces $m\n";
                close(SM);
                print "petidomo-kickout: kicked out <$m>\n";
            }
        }
    }
}
exit(0);
