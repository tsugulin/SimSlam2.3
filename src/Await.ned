simple Await
{
    parameters:
        volatile double productionTime @unit(s) = default(0);
    gates:
        input in;
        output out;
        output res;
}
