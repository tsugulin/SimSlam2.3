simple Create
{
    parameters:
        string datatype;
        volatile double initialTime @unit(s) = default(0);
        volatile double intervalTime @unit(s) = default(0);
    gates:
        output out;
}