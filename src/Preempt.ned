simple Preempt
{
    parameters:
        volatile double productionTime @unit(s) = default(0);
    gates:
        input in;
        input in_loop;
        output out;
        output res;
}
