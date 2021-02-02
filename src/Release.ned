simple Release
{
    parameters:
        volatile double intervalTime @unit(s) = default(0);
    gates:
        input in;
        output out;
        output res;
}
